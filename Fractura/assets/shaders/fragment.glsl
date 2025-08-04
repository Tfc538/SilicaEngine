/**
 * @file fragment.glsl
 * @brief Basic fragment shader for SilicaEngine demonstrations
 * @version 1.0.0
 * 
 * This fragment shader provides basic lighting and color output
 * for rendered objects.
 */

#version 460 core

// ============================================================================
// Input from Vertex Shader
// ============================================================================
in vec4 v_Color;          // Interpolated vertex color
in vec3 v_Normal;         // World space normal
in vec3 v_WorldPos;       // World space position
in vec2 v_TexCoord;       // Texture coordinates

// ============================================================================
// Uniforms
// ============================================================================
uniform vec3 u_LightPos;       // Light position (optional)
uniform vec3 u_LightColor;     // Light color (optional)
uniform vec3 u_ViewPos;        // Camera position (optional)
uniform float u_Ambient;      // Ambient light strength
uniform sampler2D u_Texture;  // Diffuse texture (optional)
uniform bool u_UseTexture;    // Whether to use texture

// ============================================================================
// Output
// ============================================================================
layout (location = 0) out vec4 FragColor;

void main() {
    // Base color from vertex or texture
    vec4 baseColor = v_Color;
    
    if (u_UseTexture) {
        baseColor *= texture(u_Texture, v_TexCoord);
    }
    
    // Simple ambient lighting
    float ambient = max(u_Ambient, 0.1);
    vec3 finalColor = baseColor.rgb * ambient;
    
    // Optional: Add simple diffuse lighting if normal is provided
    if (length(v_Normal) > 0.1) {
        vec3 lightDir = normalize(u_LightPos - v_WorldPos);
        vec3 normal = normalize(v_Normal);
        
        // Diffuse component
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * u_LightColor;
        
        finalColor = baseColor.rgb * (ambient + diffuse * 0.8);
    }
    
    // Output final color with original alpha
    FragColor = vec4(finalColor, baseColor.a);
}