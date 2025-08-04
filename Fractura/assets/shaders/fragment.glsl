/**
 * @file fragment.glsl
 * @brief Basic fragment shader for SilicaEngine
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 1.0.0
 * 
 * Provides basic lighting and color output.
 */

#version 460 core

// Input from vertex shader
in vec4 v_Color;
in vec3 v_Normal;
in vec3 v_WorldPos;
in vec2 v_TexCoord;

// Uniforms
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_ViewPos;
uniform float u_Ambient;
uniform sampler2D u_Texture;
uniform bool u_UseTexture;

// Output
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