/**
 * @file Shader.cpp
 * @brief Implementation of the Shader class for SilicaEngine
 */

#include "SilicaEngine/Renderer/Shader.h"
#include "SilicaEngine/SilicaEngine.h"  // For logging macros
#include <fstream>
#include <sstream>
#include <vector>

namespace SilicaEngine {

    Shader::Shader()
        : m_ProgramID(0) {
    }

    Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource)
        : m_ProgramID(0) {
        CreateFromString(vertexSource, fragmentSource, geometrySource);
    }

    Shader::~Shader() {
        Delete();
    }

    bool Shader::CreateFromString(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource) {
        // Delete existing program if it exists
        Delete();

        // Compile shaders
        uint32_t vertexShader = CompileShader(ShaderType::Vertex, vertexSource);
        if (vertexShader == 0) {
            SE_ERROR("Failed to compile vertex shader");
            return false;
        }

        uint32_t fragmentShader = CompileShader(ShaderType::Fragment, fragmentSource);
        if (fragmentShader == 0) {
            SE_ERROR("Failed to compile fragment shader");
            glDeleteShader(vertexShader);
            return false;
        }

        uint32_t geometryShader = 0;
        if (!geometrySource.empty()) {
            geometryShader = CompileShader(ShaderType::Geometry, geometrySource);
            if (geometryShader == 0) {
                SE_ERROR("Failed to compile geometry shader");
                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);
                return false;
            }
        }

        // Link program
        if (!LinkProgram(vertexShader, fragmentShader, geometryShader)) {
            SE_ERROR("Failed to link shader program");
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            if (geometryShader != 0) {
                glDeleteShader(geometryShader);
            }
            return false;
        }

        // Clean up individual shaders (they're now part of the program)
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        if (geometryShader != 0) {
            glDeleteShader(geometryShader);
        }

        SE_INFO("Shader program created successfully (ID: {})", m_ProgramID);
        return true;
    }

    bool Shader::CreateFromFile(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) {
        // Load shader sources from files
        std::string vertexSource = LoadShaderSource(vertexPath);
        if (vertexSource.empty()) {
            SE_ERROR("Failed to load vertex shader from file: {}", vertexPath);
            return false;
        }

        std::string fragmentSource = LoadShaderSource(fragmentPath);
        if (fragmentSource.empty()) {
            SE_ERROR("Failed to load fragment shader from file: {}", fragmentPath);
            return false;
        }

        std::string geometrySource;
        if (!geometryPath.empty()) {
            geometrySource = LoadShaderSource(geometryPath);
            if (geometrySource.empty()) {
                SE_ERROR("Failed to load geometry shader from file: {}", geometryPath);
                return false;
            }
        }

        return CreateFromString(vertexSource, fragmentSource, geometrySource);
    }

    void Shader::Bind() const {
        if (m_ProgramID != 0) {
            glUseProgram(m_ProgramID);
        }
    }

    void Shader::Unbind() const {
        glUseProgram(0);
    }

    void Shader::Delete() {
        if (m_ProgramID != 0) {
            glDeleteProgram(m_ProgramID);
            m_ProgramID = 0;
            m_UniformLocationCache.clear();
        }
    }

    bool Shader::IsValid() const {
        return m_ProgramID != 0;
    }

    uint32_t Shader::GetProgramID() const {
        return m_ProgramID;
    }

    void Shader::SetBool(const std::string& name, bool value) {
        glUniform1i(GetUniformLocationCached(name), static_cast<int>(value));
    }

    void Shader::SetInt(const std::string& name, int value) {
        glUniform1i(GetUniformLocationCached(name), value);
    }

    void Shader::SetFloat(const std::string& name, float value) {
        glUniform1f(GetUniformLocationCached(name), value);
    }

    void Shader::SetVec2(const std::string& name, const glm::vec2& value) {
        glUniform2fv(GetUniformLocationCached(name), 1, glm::value_ptr(value));
    }

    void Shader::SetVec2(const std::string& name, float x, float y) {
        glUniform2f(GetUniformLocationCached(name), x, y);
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
        glUniform3fv(GetUniformLocationCached(name), 1, glm::value_ptr(value));
    }

    void Shader::SetVec3(const std::string& name, float x, float y, float z) {
        glUniform3f(GetUniformLocationCached(name), x, y, z);
    }

    void Shader::SetVec4(const std::string& name, const glm::vec4& value) {
        glUniform4fv(GetUniformLocationCached(name), 1, glm::value_ptr(value));
    }

    void Shader::SetVec4(const std::string& name, float x, float y, float z, float w) {
        glUniform4f(GetUniformLocationCached(name), x, y, z, w);
    }

    void Shader::SetMat3(const std::string& name, const glm::mat3& mat) {
        glUniformMatrix3fv(GetUniformLocationCached(name), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& mat) {
        glUniformMatrix4fv(GetUniformLocationCached(name), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::SetIntArray(const std::string& name, int count, const int* values) {
        glUniform1iv(GetUniformLocationCached(name), count, values);
    }

    void Shader::SetFloatArray(const std::string& name, int count, const float* values) {
        glUniform1fv(GetUniformLocationCached(name), count, values);
    }

    int Shader::GetUniformLocation(const std::string& name) {
        return glGetUniformLocation(m_ProgramID, name.c_str());
    }

    std::vector<std::string> Shader::GetActiveUniforms() const {
        std::vector<std::string> uniforms;
        
        if (m_ProgramID == 0) {
            return uniforms;
        }

        GLint count;
        glGetProgramiv(m_ProgramID, GL_ACTIVE_UNIFORMS, &count);

        GLint maxLength;
        glGetProgramiv(m_ProgramID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

        std::vector<char> name(maxLength);
        for (GLint i = 0; i < count; ++i) {
            GLsizei length;
            GLint size;
            GLenum type;
            glGetActiveUniform(m_ProgramID, i, maxLength, &length, &size, &type, name.data());
            uniforms.emplace_back(name.data(), length);
        }

        return uniforms;
    }

    std::vector<std::string> Shader::GetActiveAttributes() const {
        std::vector<std::string> attributes;
        
        if (m_ProgramID == 0) {
            return attributes;
        }

        GLint count;
        glGetProgramiv(m_ProgramID, GL_ACTIVE_ATTRIBUTES, &count);

        GLint maxLength;
        glGetProgramiv(m_ProgramID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

        std::vector<char> name(maxLength);
        for (GLint i = 0; i < count; ++i) {
            GLsizei length;
            GLint size;
            GLenum type;
            glGetActiveAttrib(m_ProgramID, i, maxLength, &length, &size, &type, name.data());
            attributes.emplace_back(name.data(), length);
        }

        return attributes;
    }

    std::shared_ptr<Shader> Shader::CreateDefault() {
        const std::string vertexSource = R"(
            #version 460 core
            
            layout (location = 0) in vec3 a_Position;
            layout (location = 1) in vec4 a_Color;
            
            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;
            
            out vec4 v_Color;
            
            void main() {
                v_Color = a_Color;
                gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            }
        )";

        const std::string fragmentSource = R"(
            #version 460 core
            
            layout (location = 0) out vec4 color;
            
            in vec4 v_Color;
            
            void main() {
                color = v_Color;
            }
        )";

        auto shader = std::make_shared<Shader>();
        if (shader->CreateFromString(vertexSource, fragmentSource)) {
            return shader;
        }
        
        return nullptr;
    }

    std::string Shader::LoadShaderSource(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            SE_ERROR("Cannot open shader file: {}", filepath);
            return "";
        }

        std::stringstream ss;
        ss << file.rdbuf();
        file.close();

        return ss.str();
    }

    uint32_t Shader::CompileShader(ShaderType type, const std::string& source) {
        uint32_t shader = glCreateShader(static_cast<GLenum>(type));
        
        const char* sourceCStr = source.c_str();
        glShaderSource(shader, 1, &sourceCStr, nullptr);
        glCompileShader(shader);

        // Check for compilation errors
        std::string typeString;
        switch (type) {
            case ShaderType::Vertex: typeString = "vertex"; break;
            case ShaderType::Fragment: typeString = "fragment"; break;
            case ShaderType::Geometry: typeString = "geometry"; break;
            case ShaderType::Compute: typeString = "compute"; break;
            case ShaderType::TessControl: typeString = "tessellation control"; break;
            case ShaderType::TessEvaluation: typeString = "tessellation evaluation"; break;
        }

        if (!CheckCompileErrors(shader, typeString)) {
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    bool Shader::LinkProgram(uint32_t vertexShader, uint32_t fragmentShader, uint32_t geometryShader) {
        m_ProgramID = glCreateProgram();
        
        glAttachShader(m_ProgramID, vertexShader);
        glAttachShader(m_ProgramID, fragmentShader);
        
        if (geometryShader != 0) {
            glAttachShader(m_ProgramID, geometryShader);
        }
        
        glLinkProgram(m_ProgramID);

        if (!CheckLinkErrors()) {
            glDeleteProgram(m_ProgramID);
            m_ProgramID = 0;
            return false;
        }

        return true;
    }

    bool Shader::CheckCompileErrors(uint32_t shader, const std::string& type) {
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        
        if (!success) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            
            std::vector<char> infoLog(length);
            glGetShaderInfoLog(shader, length, &length, infoLog.data());
            
            SE_ERROR("Shader compilation error ({}):\n{}", type, infoLog.data());
            return false;
        }
        
        return true;
    }

    bool Shader::CheckLinkErrors() {
        GLint success;
        glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
        
        if (!success) {
            GLint length;
            glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &length);
            
            std::vector<char> infoLog(length);
            glGetProgramInfoLog(m_ProgramID, length, &length, infoLog.data());
            
            SE_ERROR("Shader program linking error:\n{}", infoLog.data());
            return false;
        }
        
        return true;
    }

    int Shader::GetUniformLocationCached(const std::string& name) {
        auto it = m_UniformLocationCache.find(name);
        if (it != m_UniformLocationCache.end()) {
            return it->second;
        }

        int location = glGetUniformLocation(m_ProgramID, name.c_str());
        m_UniformLocationCache[name] = location;
        
        if (location == -1) {
            SE_WARN("Uniform '{}' not found in shader program {}", name, m_ProgramID);
        }
        
        return location;
    }

} // namespace SilicaEngine