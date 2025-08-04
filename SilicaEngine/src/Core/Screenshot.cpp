/**
 * @file Screenshot.cpp
 * @brief Implementation of enhanced screenshot system
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 */

#include "SilicaEngine/Core/Screenshot.h"
#include "SilicaEngine/Core/Logger.h"
#include "SilicaEngine/Core/Window.h"
#include <glad/gl.h>

// STB Image Write for saving images
#include <stb_image_write.h>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <mutex> // Added for mutex
#include <stdexcept> // For std::bad_alloc

namespace SilicaEngine {

    // Static member initialization
    std::string Screenshot::s_DefaultDirectory = "./screenshots/";
    std::mutex Screenshot::s_DefaultDirectoryMutex;

    ScreenshotResult Screenshot::Capture(const std::string& filename, const ScreenshotConfig& config) {
        ScreenshotResult result;
        result.filename = filename;

        auto startTime = std::chrono::high_resolution_clock::now();

        // Validate configuration
        std::string errorMessage;
        if (!ValidateConfig(config, errorMessage)) {
            result.errorMessage = errorMessage;
            return result;
        }

        // Capture framebuffer data
        FramebufferData data = CaptureFramebuffer(config);
        if (data.pixels.empty()) {
            result.errorMessage = "Failed to capture framebuffer data";
            return result;
        }

        result.width = data.width;
        result.height = data.height;
        result.channels = data.channels;

        // Save to file based on format
        bool saveSuccess = false;
        switch (config.format) {
            case ScreenshotFormat::PNG:
                saveSuccess = SaveToPNG(data, filename, config.compressionLevel);
                break;
            case ScreenshotFormat::JPEG:
                saveSuccess = SaveToJPEG(data, filename, config.jpegQuality);
                break;
            case ScreenshotFormat::BMP:
                saveSuccess = SaveToBMP(data, filename);
                break;
            case ScreenshotFormat::TGA:
                saveSuccess = SaveToTGA(data, filename);
                break;
            case ScreenshotFormat::PPM:
                saveSuccess = SaveToPPM(data, filename);
                break;
            case ScreenshotFormat::RAW:
                saveSuccess = SaveToRAW(data, filename);
                break;
        }

        if (saveSuccess) {
            result.success = true;
            result.fileSize = GetFileSize(filename);
            SE_INFO("Screenshot saved: {} ({}x{}, {} bytes)", filename, result.width, result.height, result.fileSize);
        } else {
            result.errorMessage = "Failed to save screenshot to file";
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        result.captureTime = std::chrono::duration<double>(endTime - startTime).count();

        return result;
    }

    FramebufferData Screenshot::CaptureToMemory(const ScreenshotConfig& config) {
        return CaptureFramebuffer(config);
    }

    ScreenshotResult Screenshot::SaveFramebufferData(const FramebufferData& data,
                                                    const std::string& filename,
                                                    ScreenshotFormat format,
                                                    int jpegQuality) {
        ScreenshotResult result;
        result.filename = filename;
        result.width = data.width;
        result.height = data.height;
        result.channels = data.channels;

        auto startTime = std::chrono::high_resolution_clock::now();

        bool saveSuccess = false;
        switch (format) {
            case ScreenshotFormat::PNG:
                saveSuccess = SaveToPNG(data, filename, 6);
                break;
            case ScreenshotFormat::JPEG:
                saveSuccess = SaveToJPEG(data, filename, jpegQuality);
                break;
            case ScreenshotFormat::BMP:
                saveSuccess = SaveToBMP(data, filename);
                break;
            case ScreenshotFormat::TGA:
                saveSuccess = SaveToTGA(data, filename);
                break;
            case ScreenshotFormat::PPM:
                saveSuccess = SaveToPPM(data, filename);
                break;
            case ScreenshotFormat::RAW:
                saveSuccess = SaveToRAW(data, filename);
                break;
        }

        result.success = saveSuccess;
        if (saveSuccess) {
            result.fileSize = GetFileSize(filename);
        } else {
            result.errorMessage = "Failed to save framebuffer data to file";
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        result.captureTime = std::chrono::duration<double>(endTime - startTime).count();

        return result;
    }

    ScreenshotResult Screenshot::QuickCapture(ScreenshotFormat format, const std::string& prefix) {
        std::string filename = GenerateTimestampedFilename(prefix, format);
        std::string fullPath = s_DefaultDirectory + filename;
        
        EnsureDirectoryExists(s_DefaultDirectory);
        
        ScreenshotConfig config;
        config.format = format;
        config.addTimestamp = true;
        
        return Capture(fullPath, config);
    }

    ScreenshotResult Screenshot::CaptureSequence(const std::string& baseFilename,
                                                int frameNumber,
                                                const ScreenshotConfig& config) {
        std::string filename = GenerateSequenceFilename(baseFilename, frameNumber, config.format);
        return Capture(filename, config);
    }

    std::string Screenshot::GetFileExtension(ScreenshotFormat format) {
        switch (format) {
            case ScreenshotFormat::PNG: return ".png";
            case ScreenshotFormat::JPEG: return ".jpg";
            case ScreenshotFormat::BMP: return ".bmp";
            case ScreenshotFormat::TGA: return ".tga";
            case ScreenshotFormat::PPM: return ".ppm";
            case ScreenshotFormat::RAW: return ".raw";
        }
        return ".png";
    }

    std::string Screenshot::GetFormatDescription(ScreenshotFormat format) {
        switch (format) {
            case ScreenshotFormat::PNG: return "Portable Network Graphics (PNG)";
            case ScreenshotFormat::JPEG: return "JPEG Image";
            case ScreenshotFormat::BMP: return "Windows Bitmap (BMP)";
            case ScreenshotFormat::TGA: return "Targa Image (TGA)";
            case ScreenshotFormat::PPM: return "Portable Pixmap (PPM)";
            case ScreenshotFormat::RAW: return "Raw Framebuffer Data";
        }
        return "Unknown Format";
    }

    std::vector<ScreenshotFormat> Screenshot::GetSupportedFormats() {
        return {
            ScreenshotFormat::PNG,
            ScreenshotFormat::JPEG,
            ScreenshotFormat::BMP,
            ScreenshotFormat::TGA,
            ScreenshotFormat::PPM,
            ScreenshotFormat::RAW
        };
    }

    bool Screenshot::ValidateConfig(const ScreenshotConfig& config, std::string& errorMessage) {
        if (config.jpegQuality < 1 || config.jpegQuality > 100) {
            errorMessage = "JPEG quality must be between 1 and 100";
            return false;
        }
        
        if (config.compressionLevel < 0 || config.compressionLevel > 9) {
            errorMessage = "PNG compression level must be between 0 and 9";
            return false;
        }

        if (config.width < 0 || config.height < 0 || config.x < 0 || config.y < 0) {
            errorMessage = "Screenshot dimensions and position must be non-negative";
            return false;
        }

        return true;
    }

    void Screenshot::SetDefaultDirectory(const std::string& directory) {
        std::lock_guard<std::mutex> lock(s_DefaultDirectoryMutex);
        s_DefaultDirectory = directory;
        if (!s_DefaultDirectory.empty() && s_DefaultDirectory.back() != '/' && s_DefaultDirectory.back() != '\\') {
            s_DefaultDirectory += "/";
        }
    }

    const std::string& Screenshot::GetDefaultDirectory() {
        std::lock_guard<std::mutex> lock(s_DefaultDirectoryMutex);
        return s_DefaultDirectory;
    }

    std::string Screenshot::GenerateTimestampedFilename(const std::string& prefix,
                                                       ScreenshotFormat format,
                                                       const std::string& timestampFormat) {
        std::string timestamp = GetCurrentTimestamp(timestampFormat);
        return prefix + "_" + timestamp + GetFileExtension(format);
    }

    std::string Screenshot::GenerateSequenceFilename(const std::string& baseFilename,
                                                    int frameNumber,
                                                    ScreenshotFormat format,
                                                    int padding) {
        std::stringstream ss;
        ss << baseFilename << "_" << std::setfill('0') << std::setw(padding) << frameNumber << GetFileExtension(format);
        return ss.str();
    }

    // === Private Implementation ===

    FramebufferData Screenshot::CaptureFramebuffer(const ScreenshotConfig& config) {
        FramebufferData data;

        // Get framebuffer size
        int fbWidth, fbHeight;
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        fbWidth = viewport[2];
        fbHeight = viewport[3];

        // Determine capture region
        int captureX = config.x;
        int captureY = config.y;
        int captureWidth = (config.width > 0) ? config.width : fbWidth;
        int captureHeight = (config.height > 0) ? config.height : fbHeight;

        // Clamp to framebuffer bounds
        captureX = std::max(0, std::min(captureX, fbWidth - 1));
        captureY = std::max(0, std::min(captureY, fbHeight - 1));
        captureWidth = std::min(captureWidth, fbWidth - captureX);
        captureHeight = std::min(captureHeight, fbHeight - captureY);

        data.width = captureWidth;
        data.height = captureHeight;
        data.channels = config.includeAlpha ? 4 : 3;
        data.flipVertically = config.flipVertically;

        // Allocate pixel buffer
        try {
            data.pixels.resize(captureWidth * captureHeight * data.channels);
        } catch (const std::bad_alloc& e) {
            SE_ERROR("Failed to allocate memory for screenshot capture: {}x{} pixels ({} channels). Error: {}", 
                     captureWidth, captureHeight, data.channels, e.what());
            // Return empty data to indicate failure
            data.pixels.clear();
            data.width = 0;
            data.height = 0;
            data.channels = 0;
            return data;
        }

        // Read pixels from framebuffer
        GLenum format = config.includeAlpha ? GL_RGBA : GL_RGB;
        glReadPixels(captureX, captureY, captureWidth, captureHeight, format, GL_UNSIGNED_BYTE, data.pixels.data());

        // Check for OpenGL errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            SE_ERROR("OpenGL error during screenshot capture: 0x{:x}", error);
            data.pixels.clear();
            return data;
        }

        // Flip image if requested (OpenGL origin is bottom-left)
        if (data.flipVertically) {
            FlipImageVertically(data.pixels, data.width, data.height, data.channels);
        }

        return data;
    }

    bool Screenshot::SaveToPNG(const FramebufferData& data, const std::string& filename, int compression) {
        EnsureDirectoryExists(std::filesystem::path(filename).parent_path().string());
        
        stbi_write_png_compression_level = compression;
        int result = stbi_write_png(filename.c_str(), data.width, data.height, data.channels, 
                                   data.pixels.data(), data.width * data.channels);
        return result != 0;
    }

    bool Screenshot::SaveToJPEG(const FramebufferData& data, const std::string& filename, int quality) {
        EnsureDirectoryExists(std::filesystem::path(filename).parent_path().string());
        
        // JPEG doesn't support alpha channel, convert RGBA to RGB if needed
        if (data.channels == 4) {
            std::vector<unsigned char> rgbPixels(data.width * data.height * 3);
            for (int i = 0, j = 0; i < data.pixels.size(); i += 4, j += 3) {
                rgbPixels[j] = data.pixels[i];         // R
                rgbPixels[j + 1] = data.pixels[i + 1]; // G
                rgbPixels[j + 2] = data.pixels[i + 2]; // B
            }
            return stbi_write_jpg(filename.c_str(), data.width, data.height, 3, rgbPixels.data(), quality) != 0;
        } else {
            return stbi_write_jpg(filename.c_str(), data.width, data.height, data.channels, 
                                 data.pixels.data(), quality) != 0;
        }
    }

    bool Screenshot::SaveToBMP(const FramebufferData& data, const std::string& filename) {
        EnsureDirectoryExists(std::filesystem::path(filename).parent_path().string());
        
        int result = stbi_write_bmp(filename.c_str(), data.width, data.height, data.channels, data.pixels.data());
        return result != 0;
    }

    bool Screenshot::SaveToTGA(const FramebufferData& data, const std::string& filename) {
        EnsureDirectoryExists(std::filesystem::path(filename).parent_path().string());
        
        int result = stbi_write_tga(filename.c_str(), data.width, data.height, data.channels, data.pixels.data());
        return result != 0;
    }

    bool Screenshot::SaveToPPM(const FramebufferData& data, const std::string& filename) {
        EnsureDirectoryExists(std::filesystem::path(filename).parent_path().string());
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        // PPM header
        file << "P3\n" << data.width << " " << data.height << "\n255\n";

        // Write pixel data (RGB only for PPM)
        for (int i = 0; i < data.pixels.size(); i += data.channels) {
            file << static_cast<int>(data.pixels[i]) << " "
                 << static_cast<int>(data.pixels[i + 1]) << " "
                 << static_cast<int>(data.pixels[i + 2]) << " ";
            
            if ((i / data.channels + 1) % data.width == 0) {
                file << "\n";
            }
        }

        return file.good();
    }

    bool Screenshot::SaveToRAW(const FramebufferData& data, const std::string& filename) {
        EnsureDirectoryExists(std::filesystem::path(filename).parent_path().string());
        
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        // Write header with dimensions and channel info
        file.write(reinterpret_cast<const char*>(&data.width), sizeof(int));
        file.write(reinterpret_cast<const char*>(&data.height), sizeof(int));
        file.write(reinterpret_cast<const char*>(&data.channels), sizeof(int));

        // Write pixel data
        file.write(reinterpret_cast<const char*>(data.pixels.data()), data.pixels.size());

        return file.good();
    }

    void Screenshot::FlipImageVertically(std::vector<unsigned char>& pixels, int width, int height, int channels) {
        int rowSize = width * channels;
        std::vector<unsigned char> temp(rowSize);

        for (int y = 0; y < height / 2; ++y) {
            int topRowStart = y * rowSize;
            int bottomRowStart = (height - 1 - y) * rowSize;

            // Swap rows
            std::copy(pixels.begin() + topRowStart, pixels.begin() + topRowStart + rowSize, temp.begin());
            std::copy(pixels.begin() + bottomRowStart, pixels.begin() + bottomRowStart + rowSize, 
                     pixels.begin() + topRowStart);
            std::copy(temp.begin(), temp.end(), pixels.begin() + bottomRowStart);
        }
    }

    std::string Screenshot::GetCurrentTimestamp(const std::string& format) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), format.c_str());
        return ss.str();
    }

    void Screenshot::EnsureDirectoryExists(const std::string& path) {
        if (!path.empty()) {
            std::filesystem::create_directories(path);
        }
    }

    size_t Screenshot::GetFileSize(const std::string& filename) {
        std::error_code ec;
        auto size = std::filesystem::file_size(filename, ec);
        return ec ? 0 : size;
    }

    // === ScreenshotManager Implementation ===

    void ScreenshotManager::Update(double deltaTime) {
        if (m_AutoCaptureEnabled) {
            m_TimeSinceLastCapture += deltaTime;
            if (m_TimeSinceLastCapture >= m_AutoCaptureInterval) {
                std::string filename = Screenshot::GenerateTimestampedFilename("auto", m_Config.format);
                std::string fullPath = Screenshot::GetDefaultDirectory() + filename;
                
                ScreenshotResult result = Capture(fullPath);
                m_TimeSinceLastCapture = 0.0;
            }
        }
    }

    ScreenshotResult ScreenshotManager::Capture(const std::string& filename) {
        ScreenshotResult result = Screenshot::Capture(filename, m_Config);
        
        m_TotalCaptureCount++;
        if (result.success) {
            m_SuccessfulCaptureCount++;
        }
        m_TotalCaptureTime += result.captureTime;
        
        return result;
    }

    void ScreenshotManager::StartSequenceCapture(const std::string& baseFilename) {
        m_SequenceCapture = true;
        m_SequenceBaseFilename = baseFilename;
        m_SequenceFrameNumber = 0;
        SE_INFO("Started sequence capture: {}", baseFilename);
    }

    void ScreenshotManager::StopSequenceCapture() {
        if (m_SequenceCapture) {
            SE_INFO("Stopped sequence capture at frame {}", m_SequenceFrameNumber);
            m_SequenceCapture = false;
        }
    }

    double ScreenshotManager::GetAverageCaptureTime() const {
        return m_TotalCaptureCount > 0 ? m_TotalCaptureTime / m_TotalCaptureCount : 0.0;
    }

    void ScreenshotManager::ClearStatistics() {
        m_TotalCaptureCount = 0;
        m_SuccessfulCaptureCount = 0;
        m_TotalCaptureTime = 0.0;
    }

} // namespace SilicaEngine