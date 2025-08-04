/**
 * @file vertex.glsl
 * @brief Basic vertex shader for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.0.0
 * 
 * Handles 3D transformations and attribute passing.
 */

#version 460 core

// Input attributes
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec3 a_Normal;
layout (location = 3) in vec2 a_TexCoord;

// Uniforms
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform mat3 u_NormalMatrix;
uniform float u_Time;

// Output to fragment shader
out vec4 v_Color;
out vec3 v_Normal;
out vec3 v_WorldPos;
out vec2 v_TexCoord;

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