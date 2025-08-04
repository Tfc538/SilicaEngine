/**
 * @file vertex.glsl
 * @brief Basic vertex shader for SilicaEngine demonstrations
 * @version 1.0.0
 * 
 * This vertex shader handles basic 3D transformations and passes
 * vertex attributes to the fragment shader.
 */

#version 460 core

// ============================================================================
// Input Attributes
// ============================================================================
layout (location = 0) in vec3 a_Position;    // Vertex position
layout (location = 1) in vec4 a_Color;       // Vertex color
layout (location = 2) in vec3 a_Normal;      // Vertex normal (optional)
layout (location = 3) in vec2 a_TexCoord;    // Texture coordinates (optional)

// ============================================================================
// Uniforms
// ============================================================================
uniform mat4 u_ViewProjection;    // Combined view and projection matrix
uniform mat4 u_Transform;         // Model transformation matrix
uniform mat3 u_NormalMatrix;      // Normal transformation matrix (optional)
uniform float u_Time;             // Time for animations (optional)

// ============================================================================
// Output to Fragment Shader
// ============================================================================
out vec4 v_Color;          // Interpolated color
out vec3 v_Normal;         // World space normal
out vec3 v_WorldPos;       // World space position
out vec2 v_TexCoord;       // Texture coordinates

void main() {
    // Transform vertex position to world space
    vec4 worldPos = u_Transform * vec4(a_Position, 1.0);
    v_WorldPos = worldPos.xyz;
    
    // Transform to clip space
    gl_Position = u_ViewProjection * worldPos;
    
    // Pass through vertex color
    v_Color = a_Color;
    
    // Transform normal to world space (if provided)
    v_Normal = normalize(u_NormalMatrix * a_Normal);
    
    // Pass through texture coordinates
    v_TexCoord = a_TexCoord;
}