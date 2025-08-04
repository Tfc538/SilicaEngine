/**
 * @file Texture.h
 * @brief Texture management for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.1.0
 * 
 * OpenGL texture wrapper with support for various formats and loading from files.
 */

#pragma once

#include <glad/gl.h>
#include <string>
#include <memory>
#include "SilicaEngine/Core/ErrorCodes.h"

namespace SilicaEngine {

    /// Texture filtering modes
    enum class TextureFilter {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
        NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
        LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
        NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
    };

    /// Texture wrapping modes
    enum class TextureWrap {
        Repeat = GL_REPEAT,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER
    };

    /// Texture formats
    enum class TextureFormat {
        RGB = GL_RGB,
        RGBA = GL_RGBA,
        Red = GL_RED,
        RG = GL_RG,
        BGR = GL_BGR,
        BGRA = GL_BGRA,
        DepthComponent = GL_DEPTH_COMPONENT,
        StencilIndex = GL_STENCIL_INDEX
    };

    /// Internal texture formats
    enum class TextureInternalFormat {
        RGB8 = GL_RGB8,
        RGBA8 = GL_RGBA8,
        RGB16F = GL_RGB16F,
        RGBA16F = GL_RGBA16F,
        RGB32F = GL_RGB32F,
        RGBA32F = GL_RGBA32F,
        DepthComponent16 = GL_DEPTH_COMPONENT16,
        DepthComponent24 = GL_DEPTH_COMPONENT24,
        DepthComponent32F = GL_DEPTH_COMPONENT32F
    };

    /// Texture creation parameters
    struct TextureParams {
        TextureFilter minFilter = TextureFilter::Linear;
        TextureFilter magFilter = TextureFilter::Linear;
        TextureWrap wrapS = TextureWrap::Repeat;
        TextureWrap wrapT = TextureWrap::Repeat;
        bool generateMipmaps = true;
        TextureInternalFormat internalFormat = TextureInternalFormat::RGBA8;
    };

    /// OpenGL Texture wrapper class
    class Texture {
    public:
        /// Create empty texture
        Texture();
        
        /// Create texture from file
        explicit Texture(const std::string& filepath, const TextureParams& params = TextureParams{});
        
        /// Create texture from memory data
        Texture(int width, int height, TextureFormat format, const void* data, 
                const TextureParams& params = TextureParams{});
        
        /// Destructor
        ~Texture();
        
        // Non-copyable but movable
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;
        
        /// Load texture from file
        ErrorResult<void> LoadFromFile(const std::string& filepath, const TextureParams& params = TextureParams{});
        
        /// Load texture from memory data
        ErrorResult<void> LoadFromMemory(int width, int height, TextureFormat format, const void* data,
                                        const TextureParams& params = TextureParams{});
        
        /// Bind texture to specified texture unit
        void Bind(int unit = 0) const;
        
        /// Unbind texture
        void Unbind() const;
        
        /// Generate mipmaps
        void GenerateMipmaps() const;
        
        /// Update texture parameters
        void SetFilter(TextureFilter minFilter, TextureFilter magFilter);
        void SetWrap(TextureWrap wrapS, TextureWrap wrapT);
        
        /// Getters
        uint32_t GetID() const { return m_TextureID; }
        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }
        int GetChannels() const { return m_Channels; }
        const std::string& GetFilepath() const { return m_Filepath; }
        const TextureParams& GetParams() const { return m_Params; }
        bool IsValid() const { return m_TextureID != 0; }
        
        /// Static utility functions
        static std::shared_ptr<Texture> Create(const std::string& filepath, 
                                              const TextureParams& params = TextureParams{});
        static std::shared_ptr<Texture> Create(int width, int height, TextureFormat format, 
                                              const void* data, const TextureParams& params = TextureParams{});
        static std::shared_ptr<Texture> CreateWhite();
        static std::shared_ptr<Texture> CreateBlack();
        static std::shared_ptr<Texture> CreateCheckerboard(int size = 256);
        
    private:
        uint32_t m_TextureID = 0;
        int m_Width = 0;
        int m_Height = 0;
        int m_Channels = 0;
        std::string m_Filepath;
        TextureParams m_Params;
        
        void CreateTexture(const void* data, TextureFormat format);
        void ApplyParameters();
        void Release();
        
        static GLenum GetGLFormat(TextureFormat format);
        static GLenum GetGLInternalFormat(TextureInternalFormat format);
        static GLenum GetGLFilter(TextureFilter filter);
        static GLenum GetGLWrap(TextureWrap wrap);
    };

} // namespace SilicaEngine