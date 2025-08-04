/**
 * @file Shader.h
 * @brief Shader management class for SilicaEngine
 * @version 1.0.0
 * 
 * Provides functionality for loading, compiling, and using OpenGL shaders
 * with a convenient interface for setting uniforms.
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

namespace SilicaEngine {

    /**
     * @enum ShaderType
     * @brief Supported shader types
     */
    enum class ShaderType {
        Vertex = GL_VERTEX_SHADER,              ///< Vertex shader
        Fragment = GL_FRAGMENT_SHADER,          ///< Fragment shader
        Geometry = GL_GEOMETRY_SHADER,          ///< Geometry shader
        Compute = GL_COMPUTE_SHADER,            ///< Compute shader
        TessControl = GL_TESS_CONTROL_SHADER,   ///< Tessellation control shader
        TessEvaluation = GL_TESS_EVALUATION_SHADER ///< Tessellation evaluation shader
    };

    /**
     * @class Shader
     * @brief OpenGL shader program wrapper
     * 
     * Provides a high-level interface for working with OpenGL shaders,
     * including compilation, linking, and uniform management.
     */
    class Shader {
    public:
        /**
         * @brief Default constructor
         */
        Shader();

        /**
         * @brief Constructor with shader source strings
         * @param vertexSource Vertex shader source code
         * @param fragmentSource Fragment shader source code
         * @param geometrySource Optional geometry shader source code
         */
        Shader(const std::string& vertexSource, const std::string& fragmentSource, 
               const std::string& geometrySource = "");

        /**
         * @brief Destructor
         */
        ~Shader();

        /**
         * @brief Create shader from source strings
         * @param vertexSource Vertex shader source code
         * @param fragmentSource Fragment shader source code
         * @param geometrySource Optional geometry shader source code
         * @return true if creation was successful
         */
        bool CreateFromString(const std::string& vertexSource, const std::string& fragmentSource,
                            const std::string& geometrySource = "");

        /**
         * @brief Create shader from files
         * @param vertexPath Path to vertex shader file
         * @param fragmentPath Path to fragment shader file
         * @param geometryPath Optional path to geometry shader file
         * @return true if creation was successful
         */
        bool CreateFromFile(const std::string& vertexPath, const std::string& fragmentPath,
                          const std::string& geometryPath = "");

        /**
         * @brief Use this shader program
         */
        void Bind() const;

        /**
         * @brief Stop using this shader program
         */
        void Unbind() const;

        /**
         * @brief Delete the shader program
         */
        void Delete();

        /**
         * @brief Check if the shader is valid
         * @return true if the shader program is valid
         */
        bool IsValid() const;

        /**
         * @brief Get the OpenGL program ID
         * @return OpenGL program ID
         */
        uint32_t GetProgramID() const;

        // Uniform setting methods
        
        /**
         * @brief Set a boolean uniform
         * @param name Uniform name
         * @param value Boolean value
         */
        void SetBool(const std::string& name, bool value);

        /**
         * @brief Set an integer uniform
         * @param name Uniform name
         * @param value Integer value
         */
        void SetInt(const std::string& name, int value);

        /**
         * @brief Set a float uniform
         * @param name Uniform name
         * @param value Float value
         */
        void SetFloat(const std::string& name, float value);

        /**
         * @brief Set a vec2 uniform
         * @param name Uniform name
         * @param value Vec2 value
         */
        void SetVec2(const std::string& name, const glm::vec2& value);

        /**
         * @brief Set a vec2 uniform
         * @param name Uniform name
         * @param x X component
         * @param y Y component
         */
        void SetVec2(const std::string& name, float x, float y);

        /**
         * @brief Set a vec3 uniform
         * @param name Uniform name
         * @param value Vec3 value
         */
        void SetVec3(const std::string& name, const glm::vec3& value);

        /**
         * @brief Set a vec3 uniform
         * @param name Uniform name
         * @param x X component
         * @param y Y component
         * @param z Z component
         */
        void SetVec3(const std::string& name, float x, float y, float z);

        /**
         * @brief Set a vec4 uniform
         * @param name Uniform name
         * @param value Vec4 value
         */
        void SetVec4(const std::string& name, const glm::vec4& value);

        /**
         * @brief Set a vec4 uniform
         * @param name Uniform name
         * @param x X component
         * @param y Y component
         * @param z Z component
         * @param w W component
         */
        void SetVec4(const std::string& name, float x, float y, float z, float w);

        /**
         * @brief Set a mat3 uniform
         * @param name Uniform name
         * @param mat Matrix value
         */
        void SetMat3(const std::string& name, const glm::mat3& mat);

        /**
         * @brief Set a mat4 uniform
         * @param name Uniform name
         * @param mat Matrix value
         */
        void SetMat4(const std::string& name, const glm::mat4& mat);

        /**
         * @brief Set an integer array uniform
         * @param name Uniform name
         * @param count Number of elements
         * @param values Array of values
         */
        void SetIntArray(const std::string& name, int count, const int* values);

        /**
         * @brief Set a float array uniform
         * @param name Uniform name
         * @param count Number of elements
         * @param values Array of values
         */
        void SetFloatArray(const std::string& name, int count, const float* values);

        /**
         * @brief Get the location of a uniform
         * @param name Uniform name
         * @return Uniform location (-1 if not found)
         */
        int GetUniformLocation(const std::string& name);

        /**
         * @brief Get all active uniform names
         * @return Vector of uniform names
         */
        std::vector<std::string> GetActiveUniforms() const;

        /**
         * @brief Get all active attribute names
         * @return Vector of attribute names
         */
        std::vector<std::string> GetActiveAttributes() const;

        /**
         * @brief Create a default shader with basic vertex and fragment shaders
         * @return Shared pointer to the default shader
         */
        static std::shared_ptr<Shader> CreateDefault();

        /**
         * @brief Load shader source from file
         * @param filepath Path to the shader file
         * @return Shader source code
         */
        static std::string LoadShaderSource(const std::string& filepath);

    private:
        uint32_t m_ProgramID;                                       ///< OpenGL program ID
        std::unordered_map<std::string, int> m_UniformLocationCache; ///< Cache for uniform locations

        /**
         * @brief Compile a shader
         * @param type Shader type
         * @param source Shader source code
         * @return Compiled shader ID (0 on failure)
         */
        uint32_t CompileShader(ShaderType type, const std::string& source);

        /**
         * @brief Link the shader program
         * @param vertexShader Vertex shader ID
         * @param fragmentShader Fragment shader ID
         * @param geometryShader Geometry shader ID (0 if not used)
         * @return true if linking was successful
         */
        bool LinkProgram(uint32_t vertexShader, uint32_t fragmentShader, uint32_t geometryShader = 0);

        /**
         * @brief Check for shader compilation errors
         * @param shader Shader ID
         * @param type Shader type for error reporting
         * @return true if no errors
         */
        bool CheckCompileErrors(uint32_t shader, const std::string& type);

        /**
         * @brief Check for program linking errors
         * @return true if no errors
         */
        bool CheckLinkErrors();

        /**
         * @brief Get uniform location with caching
         * @param name Uniform name
         * @return Uniform location
         */
        int GetUniformLocationCached(const std::string& name);
    };

} // namespace SilicaEngine