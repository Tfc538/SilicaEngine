/**
 * @file Texture.cpp
 * @brief Implementation of texture management for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 */

#include "SilicaEngine/Renderer/Texture.h"
#include "SilicaEngine/Core/ErrorCodes.h"
#include "SilicaEngine/Core/Logger.h"

// STB Image for loading texture files (implementation in separate file)
#include <stb_image.h>

#include <algorithm>
#include <vector>

namespace SilicaEngine {

    Texture::Texture() = default;

    Texture::Texture(const std::string& filepath, const TextureParams& params)
        : m_Params(params) {
        LoadFromFile(filepath, params);
    }

    Texture::Texture(int width, int height, TextureFormat format, const void* data, const TextureParams& params)
        : m_Width(width), m_Height(height), m_Params(params) {
        LoadFromMemory(width, height, format, data, params);
    }

    Texture::~Texture() {
        Release();
    }

    Texture::Texture(Texture&& other) noexcept
        : m_TextureID(other.m_TextureID)
        , m_Width(other.m_Width)
        , m_Height(other.m_Height)
        , m_Channels(other.m_Channels)
        , m_Filepath(std::move(other.m_Filepath))
        , m_Params(other.m_Params) {
        other.m_TextureID = 0;
        other.m_Width = 0;
        other.m_Height = 0;
        other.m_Channels = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        if (this != &other) {
            Release();
            
            m_TextureID = other.m_TextureID;
            m_Width = other.m_Width;
            m_Height = other.m_Height;
            m_Channels = other.m_Channels;
            m_Filepath = std::move(other.m_Filepath);
            m_Params = other.m_Params;
            
            other.m_TextureID = 0;
            other.m_Width = 0;
            other.m_Height = 0;
            other.m_Channels = 0;
        }
        return *this;
    }

    ErrorResult<void> Texture::LoadFromFile(const std::string& filepath, const TextureParams& params) {
        Release();
        
        m_Filepath = filepath;
        m_Params = params;
        
        // Thread-safe texture loading: use mutex to protect global stbi state
        static std::mutex s_StbiMutex;
        std::lock_guard<std::mutex> lock(s_StbiMutex);
        
        // Set STB to flip images vertically (OpenGL expects bottom-left origin)
        stbi_set_flip_vertically_on_load(true);
        
        // Load image data
        unsigned char* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_Channels, 0);
        if (!data) {
            SE_ERROR("Failed to load texture: {} - {}", filepath, stbi_failure_reason());
            return ErrorResult<void>::Error(EngineError::TextureLoadFailed, "Failed to load texture: " + filepath + " - " + stbi_failure_reason());
        }
        
        // Determine format based on channels
        TextureFormat format;
        switch (m_Channels) {
            case 1: format = TextureFormat::Red; break;
            case 2: format = TextureFormat::RG; break;
            case 3: format = TextureFormat::RGB; break;
            case 4: format = TextureFormat::RGBA; break;
            default:
                SE_ERROR("Unsupported number of channels: {} in texture: {}", m_Channels, filepath);
                stbi_image_free(data);
                return ErrorResult<void>::Error(EngineError::TextureLoadFailed, "Unsupported number of channels: " + std::to_string(m_Channels) + " in texture: " + filepath);
        }
        
        // Create OpenGL texture
        CreateTexture(data, format);
        
        // Free image data
        stbi_image_free(data);
        
        if (m_TextureID != 0) {
            SE_INFO("Texture loaded successfully: {} ({}x{}, {} channels)", filepath, m_Width, m_Height, m_Channels);
            return ErrorResult<void>::Success();
        }
        
        return ErrorResult<void>::Error(EngineError::TextureLoadFailed, "Failed to create OpenGL texture for: " + filepath);
    }

    ErrorResult<void> Texture::LoadFromMemory(int width, int height, TextureFormat format, const void* data, const TextureParams& params) {
        Release();
        
        m_Width = width;
        m_Height = height;
        m_Params = params;
        m_Filepath = "<memory>";
        
        // Determine channels from format
        switch (format) {
            case TextureFormat::Red: m_Channels = 1; break;
            case TextureFormat::RG: m_Channels = 2; break;
            case TextureFormat::RGB:
            case TextureFormat::BGR: m_Channels = 3; break;
            case TextureFormat::RGBA:
            case TextureFormat::BGRA: m_Channels = 4; break;
            default: m_Channels = 4; break;
        }
        
        CreateTexture(data, format);
        
        if (m_TextureID != 0) {
            SE_INFO("Texture created from memory: {}x{}, {} channels", width, height, m_Channels);
            return ErrorResult<void>::Success();
        }
        
        return ErrorResult<void>::Error(EngineError::TextureLoadFailed, "Failed to create texture from memory data");
    }

    void Texture::Bind(int unit) const {
        if (m_TextureID == 0) {
            SE_WARN("Attempting to bind invalid texture");
            return;
        }
        
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }

    void Texture::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::GenerateMipmaps() const {
        if (m_TextureID == 0) return;
        
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Texture::SetFilter(TextureFilter minFilter, TextureFilter magFilter) {
        if (m_TextureID == 0) return;
        
        m_Params.minFilter = minFilter;
        m_Params.magFilter = magFilter;
        
        // Save currently bound texture to restore after parameter changes
        GLint previousTexture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
        
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGLFilter(minFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGLFilter(magFilter));
        
        // Restore previous texture binding
        glBindTexture(GL_TEXTURE_2D, previousTexture);
    }

    void Texture::SetWrap(TextureWrap wrapS, TextureWrap wrapT) {
        if (m_TextureID == 0) return;
        
        m_Params.wrapS = wrapS;
        m_Params.wrapT = wrapT;
        
        // Save currently bound texture to restore after parameter changes
        GLint previousTexture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
        
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGLWrap(wrapS));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGLWrap(wrapT));
        
        // Restore previous texture binding
        glBindTexture(GL_TEXTURE_2D, previousTexture);
    }

    std::shared_ptr<Texture> Texture::Create(const std::string& filepath, const TextureParams& params) {
        return std::make_shared<Texture>(filepath, params);
    }

    std::shared_ptr<Texture> Texture::Create(int width, int height, TextureFormat format, const void* data, const TextureParams& params) {
        return std::make_shared<Texture>(width, height, format, data, params);
    }

    std::shared_ptr<Texture> Texture::CreateWhite() {
        uint32_t whiteData = 0xFFFFFFFF;
        return Create(1, 1, TextureFormat::RGBA, &whiteData);
    }

    std::shared_ptr<Texture> Texture::CreateBlack() {
        uint32_t blackData = 0xFF000000;
        return Create(1, 1, TextureFormat::RGBA, &blackData);
    }

    std::shared_ptr<Texture> Texture::CreateCheckerboard(int size) {
        std::vector<uint8_t> data(size * size * 4);
        
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                int index = (y * size + x) * 4;
                bool isWhite = ((x / 8) + (y / 8)) % 2 == 0;
                uint8_t color = isWhite ? 255 : 0;
                
                data[index + 0] = color;     // R
                data[index + 1] = color;     // G
                data[index + 2] = color;     // B
                data[index + 3] = 255;       // A
            }
        }
        
        return Create(size, size, TextureFormat::RGBA, data.data());
    }

    void Texture::CreateTexture(const void* data, TextureFormat format) {
        glGenTextures(1, &m_TextureID);
        if (m_TextureID == 0) {
            SE_ERROR("Failed to generate OpenGL texture");
            return;
        }
        
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        
        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GetGLInternalFormat(m_Params.internalFormat),
                     m_Width, m_Height, 0, GetGLFormat(format), GL_UNSIGNED_BYTE, data);
        
        // Apply parameters
        ApplyParameters();
        
        // Generate mipmaps if requested
        if (m_Params.generateMipmaps) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        
        // Check for OpenGL errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            SE_ERROR("OpenGL error while creating texture: 0x{:x}", error);
            Release();
        }
    }

    void Texture::ApplyParameters() {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGLFilter(m_Params.minFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGLFilter(m_Params.magFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGLWrap(m_Params.wrapS));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGLWrap(m_Params.wrapT));
    }

    void Texture::Release() {
        if (m_TextureID != 0) {
            glDeleteTextures(1, &m_TextureID);
            m_TextureID = 0;
        }
        m_Width = 0;
        m_Height = 0;
        m_Channels = 0;
        m_Filepath.clear();
    }

    GLenum Texture::GetGLFormat(TextureFormat format) {
        return static_cast<GLenum>(format);
    }

    GLenum Texture::GetGLInternalFormat(TextureInternalFormat format) {
        return static_cast<GLenum>(format);
    }

    GLenum Texture::GetGLFilter(TextureFilter filter) {
        return static_cast<GLenum>(filter);
    }

    GLenum Texture::GetGLWrap(TextureWrap wrap) {
        return static_cast<GLenum>(wrap);
    }

} // namespace SilicaEngine