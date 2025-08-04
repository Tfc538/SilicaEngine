/**
 * @file vertex.glsl
 * @brief Enhanced vertex shader for Fractura: Shatter Garden
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 2.0.0
 * 
 * Handles 3D transformations with procedural vertex animation.
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

// Simple noise function for vertex displacement
float noise(vec3 pos) {
    return fract(sin(dot(pos, vec3(12.9898, 78.233, 54.53))) * 43758.5453);
}

void main() {
    // Start with original position
    vec3 animatedPosition = a_Position;
    
    // Transform to world space first
    vec4 worldPos = u_Transform * vec4(animatedPosition, 1.0);
    
    // Enhanced vertex animation for dramatic visual effects
    float noiseValue = noise(worldPos.xyz * 0.05);
    float timeOffset = u_Time * 0.8 + noiseValue * 6.28;
    
    // Multi-layered vertex displacement for complex movement
    vec3 displacement = vec3(
        sin(timeOffset) * 0.05 + sin(timeOffset * 2.0) * 0.02,
        cos(timeOffset * 1.3) * 0.03 + sin(timeOffset * 3.0) * 0.015,
        sin(timeOffset * 0.7) * 0.05 + cos(timeOffset * 2.5) * 0.02
    );
    
    // Add breathing/pulsing effect based on distance from center
    float distanceFromCenter = length(worldPos.xz);
    float breathingEffect = sin(u_Time * 1.5 - distanceFromCenter * 0.01) * 0.03;
    displacement.y += breathingEffect;
    
    // Apply enhanced displacement
    worldPos.xyz += displacement;
    v_WorldPos = worldPos.xyz;
    
    // Transform to clip space
    gl_Position = u_ViewProjection * worldPos;
    
    // Enhanced color animation with multi-frequency pulsing
    float colorPulse1 = sin(u_Time * 2.0 + noiseValue * 6.28) * 0.15 + 0.85;
    float colorPulse2 = cos(u_Time * 1.5 + distanceFromCenter * 0.1) * 0.1 + 0.9;
    float colorPulse3 = sin(u_Time * 3.0 + worldPos.y * 0.2) * 0.08 + 0.92;
    
    v_Color = a_Color * colorPulse1 * colorPulse2 * colorPulse3 * 1.1; // Boost brightness
    
    // Transform normal to world space
    v_Normal = normalize(u_NormalMatrix * a_Normal);
    
    // Pass through texture coordinates
    v_TexCoord = a_TexCoord;
}