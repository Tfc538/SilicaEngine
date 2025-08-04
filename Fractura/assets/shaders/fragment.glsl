/**
 * @file fragment.glsl
 * @brief Ultra-Enhanced fragment shader with Bloom/Glow for Fractura: Shatter Garden
 * @author Tim Gatzke <post@tim-gatzke.de>
 * @version 3.0.0
 * 
 * Provides atmospheric lighting, intense glow effects, and bloom for the massive cube garden.
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
uniform float u_Time;
uniform sampler2D u_Texture;
uniform bool u_UseTexture;

// Output
layout (location = 0) out vec4 FragColor;

void main() {
    // Base color from vertex
    vec4 baseColor = v_Color;
    
    if (u_UseTexture) {
        baseColor *= texture(u_Texture, v_TexCoord);
    }
    
    // Ultra-enhanced ambient lighting with pulsing atmosphere
    float timePulse = sin(u_Time * 0.5) * 0.1 + 0.9;
    float ambientStrength = 0.3 * timePulse;
    vec3 ambient = ambientStrength * vec3(0.9, 0.95, 1.0);
    
    // Massive emissive glow based on color intensity and time
    float colorIntensity = (baseColor.r + baseColor.g + baseColor.b) / 3.0;
    float glowPulse = sin(u_Time * 2.0 + v_WorldPos.x * 0.1 + v_WorldPos.z * 0.1) * 0.3 + 0.7;
    vec3 emissive = baseColor.rgb * colorIntensity * glowPulse * 1.5; // Massive glow boost
    
    // Distance-based atmospheric effect with enhanced glow falloff
    vec3 viewDir = normalize(u_ViewPos - v_WorldPos);
    float distance = length(u_ViewPos - v_WorldPos);
    float fog = 1.0 / (1.0 + distance * 0.005); // Slower falloff for better glow
    
    // Multi-layer lighting system
    vec3 finalColor = baseColor.rgb;
    
    if (length(v_Normal) > 0.1) {
        // Dynamic light positions that move over time
        vec3 lightPos1 = vec3(sin(u_Time * 0.3) * 50.0, 30.0, cos(u_Time * 0.3) * 50.0);
        vec3 lightPos2 = vec3(-sin(u_Time * 0.2) * 40.0, 25.0, -cos(u_Time * 0.2) * 40.0);
        vec3 lightPos3 = vec3(cos(u_Time * 0.4) * 60.0, 35.0, sin(u_Time * 0.4) * 60.0);
        
        // Vibrant, pulsing light colors
        vec3 lightColor1 = vec3(1.2, 0.8, 0.6) * (sin(u_Time * 1.5) * 0.2 + 0.8); // Warm pulsing
        vec3 lightColor2 = vec3(0.6, 0.8, 1.4) * (cos(u_Time * 1.2) * 0.2 + 0.8); // Cool pulsing
        vec3 lightColor3 = vec3(1.0, 1.2, 0.8) * (sin(u_Time * 1.8) * 0.2 + 0.8); // Golden pulsing
        
        vec3 normal = normalize(v_Normal);
        
        // Multi-layer lighting
        vec3 lightDir1 = normalize(lightPos1 - v_WorldPos);
        float diff1 = max(dot(normal, lightDir1), 0.0);
        vec3 diffuse1 = diff1 * lightColor1 * 0.8;
        
        vec3 lightDir2 = normalize(lightPos2 - v_WorldPos);
        float diff2 = max(dot(normal, lightDir2), 0.0);
        vec3 diffuse2 = diff2 * lightColor2 * 0.6;
        
        vec3 lightDir3 = normalize(lightPos3 - v_WorldPos);
        float diff3 = max(dot(normal, lightDir3), 0.0);
        vec3 diffuse3 = diff3 * lightColor3 * 0.4;
        
        // Enhanced rim lighting with time-based intensity
        float rimPower = 1.5;
        float rim = 1.0 - max(dot(viewDir, normal), 0.0);
        rim = pow(rim, rimPower);
        float rimIntensity = sin(u_Time * 3.0 + distance * 0.01) * 0.3 + 1.0;
        vec3 rimColor = baseColor.rgb * rim * rimIntensity * 1.2;
        
        // Fresnel effect for additional glow
        float fresnel = pow(1.0 - max(dot(viewDir, normal), 0.0), 3.0);
        vec3 fresnelGlow = baseColor.rgb * fresnel * 0.8;
        
        finalColor = baseColor.rgb * (ambient + diffuse1 + diffuse2 + diffuse3) + 
                     emissive + rimColor + fresnelGlow;
    } else {
        finalColor = baseColor.rgb * ambient + emissive;
    }
    
    // Enhanced atmospheric effects
    finalColor *= fog;
    
    // HDR tone mapping preparation
    finalColor = pow(finalColor, vec3(0.85)); // Gamma adjustment for better glow
    
    // Integrated bloom effect - add bright areas directly to the main color
    float brightness = dot(finalColor, vec3(0.2126, 0.7152, 0.0722)); // Luminance
    float bloomThreshold = 0.6;
    
    // Add glow effect for bright areas
    if (brightness > bloomThreshold) {
        float bloomIntensity = (brightness - bloomThreshold) / (1.0 - bloomThreshold);
        finalColor += finalColor * bloomIntensity * 0.8; // Strong glow effect
    }
    
    // Final ultra-vibrant output
    FragColor = vec4(finalColor * 1.2, baseColor.a); // Boost overall brightness
}