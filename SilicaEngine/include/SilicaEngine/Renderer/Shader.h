/**
 * @file Shader.h
 * @brief OpenGL shader management for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.0.0
 * 
 * Shader compilation, linking, and uniform management.
 */

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

namespace SilicaEngine {

    /// Supported OpenGL shader types
    enum class ShaderType {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
        Compute = GL_COMPUTE_SHADER,
        TessControl = GL_TESS_CONTROL_SHADER,
        TessEvaluation = GL_TESS_EVALUATION_SHADER
    };

    /// OpenGL shader program wrapper with compilation and uniform management
    class Shader {
    public:
        Shader();
        Shader(const std::string& vertexSource, const std::string& fragmentSource, 
               const std::string& geometrySource = "");
        ~Shader();

        /// Create shader from source strings
        bool CreateFromString(const std::string& vertexSource, const std::string& fragmentSource,
                            const std::string& geometrySource = "");

        /// Create shader from files
        bool CreateFromFile(const std::string& vertexPath, const std::string& fragmentPath,
                          const std::string& geometryPath = "");

        /// Bind/unbind shader program
        void Bind() const;
        void Unbind() const;
        void Delete();
        bool IsValid() const;
        uint32_t GetProgramID() const;

        // Uniform setters
        void SetBool(const std::string& name, bool value);
        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetVec2(const std::string& name, const glm::vec2& value);
        void SetVec2(const std::string& name, float x, float y);
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetVec3(const std::string& name, float x, float y, float z);
        void SetVec4(const std::string& name, const glm::vec4& value);
        void SetVec4(const std::string& name, float x, float y, float z, float w);
        void SetMat3(const std::string& name, const glm::mat3& mat);
        void SetMat4(const std::string& name, const glm::mat4& mat);
        void SetIntArray(const std::string& name, int count, const int* values);
        void SetFloatArray(const std::string& name, int count, const float* values);

        // Introspection
        int GetUniformLocation(const std::string& name);
        std::vector<std::string> GetActiveUniforms() const;
        std::vector<std::string> GetActiveAttributes() const;

        // Static utilities
        static std::shared_ptr<Shader> CreateDefault();
        static std::string LoadShaderSource(const std::string& filepath);

    private:
        uint32_t m_ProgramID;
        std::unordered_map<std::string, int> m_UniformLocationCache;

        uint32_t CompileShader(ShaderType type, const std::string& source);
        bool LinkProgram(uint32_t vertexShader, uint32_t fragmentShader, uint32_t geometryShader = 0);
        bool CheckCompileErrors(uint32_t shader, const std::string& type);
        bool CheckLinkErrors();
        int GetUniformLocationCached(const std::string& name);
    };

} // namespace SilicaEngine