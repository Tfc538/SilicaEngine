/**
 * @file Screenshot.h
 * @brief Enhanced screenshot system with multiple format support
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * Screenshot capture with support for PNG, JPEG, BMP, TGA, and raw framebuffer formats.
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>

namespace SilicaEngine {

    /// Supported screenshot file formats
    enum class ScreenshotFormat {
        PNG,           // Portable Network Graphics (recommended)
        JPEG,          // JPEG with configurable quality
        BMP,           // Windows Bitmap
        TGA,           // Targa
        PPM,           // Portable Pixmap (simple text format)
        RAW            // Raw framebuffer data
    };

    /// Screenshot configuration
    struct ScreenshotConfig {
        ScreenshotFormat format = ScreenshotFormat::PNG;
        int jpegQuality = 90;                    // JPEG quality (1-100)
        bool flipVertically = true;              // Flip image vertically (OpenGL uses bottom-left origin)
        bool includeAlpha = false;               // Include alpha channel (for formats that support it)
        int compressionLevel = 6;                // PNG compression level (0-9)
        
        // Region capture (0,0,0,0 means full screen)
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
        
        // Timestamp options
        bool addTimestamp = false;               // Add timestamp to filename
        std::string timestampFormat = "%Y%m%d_%H%M%S";  // strftime format
    };

    /// Screenshot capture result
    struct ScreenshotResult {
        bool success = false;
        std::string filename;
        std::string errorMessage;
        size_t fileSize = 0;
        int width = 0;
        int height = 0;
        int channels = 0;
        double captureTime = 0.0;               // Time taken to capture in seconds
    };

    /// Raw framebuffer data
    struct FramebufferData {
        std::vector<unsigned char> pixels;
        int width = 0;
        int height = 0;
        int channels = 0;                       // 3 for RGB, 4 for RGBA
        bool flipVertically = true;
    };

    /// Enhanced screenshot capture system
    class Screenshot {
    public:
        /// Capture screenshot with specified configuration
        static ScreenshotResult Capture(const std::string& filename, 
                                       const ScreenshotConfig& config = ScreenshotConfig{});

        /// Capture screenshot to memory (raw framebuffer data)
        static FramebufferData CaptureToMemory(const ScreenshotConfig& config = ScreenshotConfig{});

        /// Save raw framebuffer data to file
        static ScreenshotResult SaveFramebufferData(const FramebufferData& data,
                                                   const std::string& filename,
                                                   ScreenshotFormat format = ScreenshotFormat::PNG,
                                                   int jpegQuality = 90);

        /// Quick capture with automatic filename generation
        static ScreenshotResult QuickCapture(ScreenshotFormat format = ScreenshotFormat::PNG,
                                            const std::string& prefix = "screenshot");

        /// Capture screenshot sequence for animation/timelapse
        static ScreenshotResult CaptureSequence(const std::string& baseFilename,
                                               int frameNumber,
                                               const ScreenshotConfig& config = ScreenshotConfig{});

        /// Get file extension for format
        static std::string GetFileExtension(ScreenshotFormat format);

        /// Get format description
        static std::string GetFormatDescription(ScreenshotFormat format);

        /// Get supported formats
        static std::vector<ScreenshotFormat> GetSupportedFormats();

        /// Validate configuration
        static bool ValidateConfig(const ScreenshotConfig& config, std::string& errorMessage);

        /// Set default screenshot directory
        static void SetDefaultDirectory(const std::string& directory);
        static const std::string& GetDefaultDirectory();

        /// Utility functions
        static std::string GenerateTimestampedFilename(const std::string& prefix,
                                                      ScreenshotFormat format,
                                                      const std::string& timestampFormat = "%Y%m%d_%H%M%S");

        static std::string GenerateSequenceFilename(const std::string& baseFilename,
                                                   int frameNumber,
                                                   ScreenshotFormat format,
                                                   int padding = 4);

    private:
        static std::string s_DefaultDirectory;
        static std::mutex s_DefaultDirectoryMutex;

        // Internal capture functions
        static FramebufferData CaptureFramebuffer(const ScreenshotConfig& config);
        static bool SaveToPNG(const FramebufferData& data, const std::string& filename, int compression);
        static bool SaveToJPEG(const FramebufferData& data, const std::string& filename, int quality);
        static bool SaveToBMP(const FramebufferData& data, const std::string& filename);
        static bool SaveToTGA(const FramebufferData& data, const std::string& filename);
        static bool SaveToPPM(const FramebufferData& data, const std::string& filename);
        static bool SaveToRAW(const FramebufferData& data, const std::string& filename);

        // Utility functions
        static void FlipImageVertically(std::vector<unsigned char>& pixels, int width, int height, int channels);
        static std::string GetCurrentTimestamp(const std::string& format);
        static void EnsureDirectoryExists(const std::string& path);
        static size_t GetFileSize(const std::string& filename);
    };

    /// Screenshot manager for batch operations and automatic capture
    class ScreenshotManager {
    public:
        ScreenshotManager() = default;
        ~ScreenshotManager() = default;

        /// Set configuration for all captures
        void SetConfig(const ScreenshotConfig& config) { m_Config = config; }
        const ScreenshotConfig& GetConfig() const { return m_Config; }

        /// Enable/disable automatic capture
        void SetAutoCaptureEnabled(bool enabled) { m_AutoCaptureEnabled = enabled; }
        bool IsAutoCaptureEnabled() const { return m_AutoCaptureEnabled; }

        /// Set automatic capture interval (in seconds)
        void SetAutoCaptureInterval(double interval) { m_AutoCaptureInterval = interval; }
        double GetAutoCaptureInterval() const { return m_AutoCaptureInterval; }

        /// Update manager (call once per frame)
        void Update(double deltaTime);

        /// Capture screenshot with current configuration
        ScreenshotResult Capture(const std::string& filename);

        /// Start/stop sequence capture
        void StartSequenceCapture(const std::string& baseFilename);
        void StopSequenceCapture();
        bool IsCapturingSequence() const { return m_SequenceCapture; }

        /// Get capture statistics
        int GetTotalCaptureCount() const { return m_TotalCaptureCount; }
        int GetSuccessfulCaptureCount() const { return m_SuccessfulCaptureCount; }
        double GetAverageCaptureTime() const;

        /// Clear statistics
        void ClearStatistics();

    private:
        ScreenshotConfig m_Config;
        bool m_AutoCaptureEnabled = false;
        double m_AutoCaptureInterval = 1.0;
        double m_TimeSinceLastCapture = 0.0;
        
        bool m_SequenceCapture = false;
        std::string m_SequenceBaseFilename;
        int m_SequenceFrameNumber = 0;
        
        // Statistics
        int m_TotalCaptureCount = 0;
        int m_SuccessfulCaptureCount = 0;
        double m_TotalCaptureTime = 0.0;
        
        // Thread safety
        mutable std::mutex m_Mutex;
    };

} // namespace SilicaEngine