#version 430 core

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gPosition; // w is depth

uniform vec3 cameraWorldPos;
uniform float elapsedTime;

#include "../common/sky.glsl"
#include "../common/clouds.glsl"

#include "../common/blinn_phong.glsl"

uniform sampler2D albedoMap;
// r is block id / 255.0
// g is block shininess / 255.0
uniform sampler2D blockSampler;

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 cameraPosition;

in vec3 pos;
in vec2 texCoord;
in vec3 color;
in vec3 normal;

// Water shader parameters
const float WAVE_AMPLITUDE = 0.08;
const float WAVE_FREQUENCY = 0.6;
const float WAVE_SPEED = 0.4;
const int WAVE_OCTAVES = 3;
const float WAVE_PERSISTENCE = 0.6;
const float WAVE_LACUNARITY = 2.1;

// Simple hash function for pseudo-random values
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// Simple noise function
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    vec2 u = f * f * (3.0 - 2.0 * f);
    
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// Generate water wave height using layered noise
float getWaveHeight(vec2 worldPos, float time) {
    float height = 0.0;
    float amplitude = WAVE_AMPLITUDE;
    float frequency = WAVE_FREQUENCY;
    float speed = WAVE_SPEED;
    
    for (int i = 0; i < WAVE_OCTAVES; i++) {
        // Create directional waves
        vec2 direction = normalize(vec2(cos(float(i) * 1.2), sin(float(i) * 0.8)));
        float wave = sin(dot(direction, worldPos) * frequency - time * speed) * amplitude;
        
        // Add some noise for more organic look
        float noiseScale = frequency * 0.5;
        wave += noise(worldPos * noiseScale - vec2(time * speed * 0.3)) * amplitude * 0.3;
        
        height += wave;
        
        // Update parameters for next octave
        amplitude *= WAVE_PERSISTENCE;
        frequency *= WAVE_LACUNARITY;
        speed *= 1.2;
    }
    
    return height;
}

// Calculate water surface normal using finite differences
vec3 calculateWaterNormal(vec2 worldPos, float time) {
    const float h = 0.01; // Step size for finite differences
    
    // Sample wave heights at neighboring points
    float heightCenter = getWaveHeight(worldPos, time);
    float heightRight = getWaveHeight(worldPos + vec2(h, 0.0), time);
    float heightUp = getWaveHeight(worldPos + vec2(0.0, h), time);
    
    // Calculate gradient
    vec3 tangentX = vec3(h, heightRight - heightCenter, 0.0);
    vec3 tangentZ = vec3(0.0, heightUp - heightCenter, h);
    
    // Cross product to get normal
    vec3 waterNormal = normalize(cross(tangentX, tangentZ));
    
    return waterNormal;
}

// Calculate water color based on depth and lighting
vec3 calculateWaterColor(vec3 viewDir, vec3 waterNormal, float depth) {
    // Base water colors - shallow to deep
    vec3 shallowColor = vec3(0.4, 0.7, 0.9);  // Light blue-green
    vec3 deepColor = vec3(0.0, 0.3, 0.6);     // Dark blue
    
    // Depth-based color mixing (using fragment depth as proxy)
    float depthFactor = smoothstep(0.0, 1.0, depth);
    vec3 baseColor = mix(shallowColor, deepColor, depthFactor);
    
    // Add subsurface scattering effect
    float scattering = pow(max(0.0, dot(lightDirection, -viewDir)), 2.0);
    vec3 scatterColor = lightColor * scattering * 0.4;
    
    // Add slight foam/bubble effect in shallow areas
    float foam = (1.0 - depthFactor) * 0.2;
    
    return baseColor + scatterColor + vec3(foam);
}

// Enhanced Fresnel calculation with physically-based parameters
float calculateFresnel(vec3 viewDir, vec3 normal) {
    float cosTheta = max(0.0, dot(viewDir, normal));
    
    // Water has IOR of ~1.33, so F0 = ((1-1.33)/(1+1.33))^2 ≈ 0.02
    float f0 = 0.02;
    
    // Schlick's approximation with power adjustment for water
    float fresnel = f0 + (1.0 - f0) * pow(1.0 - cosTheta, 3.0);
    
    // Boost fresnel for more dramatic effect
    return clamp(fresnel, 0.02, 0.95);
}

// Add animated surface detail using texture coordinates
vec3 addSurfaceDetail(vec3 baseNormal, vec2 pos, float time) {
    // Create animated surface ripples using texture coordinates
    vec2 ripple1 = pos * 2.0 + vec2(time * 0.03, time * 0.02);
    vec2 ripple2 = pos * 4.0 + vec2(-time * 0.02, time * 0.035);
    
    // Generate ripple normals using noise derivatives
    float rippleHeight1 = noise(ripple1) * 0.1;
    float rippleHeight2 = noise(ripple2) * 0.06;
    
    // Simple normal perturbation
    vec3 rippleNormal = vec3(
        (rippleHeight1 + rippleHeight2) * 0.5,
        1.0,
        (rippleHeight1 - rippleHeight2) * 0.3
    );
    
    // Blend with base normal
    return normalize(baseNormal + rippleNormal * 0.3);
}

void main()
{
    vec4 tex = texture(albedoMap, texCoord);
    vec4 blockData = texture(blockSampler, texCoord);
    
    int blockID = int(blockData.r * 255.0);
    float shininess = blockData.g * 255.0;
    
    // Handle water blocks (blockID == 4)
    if(blockID == 4) 
    {
        vec3 worldPos = cameraWorldPos + pos;

        // Calculate animated water surface normal
        vec3 waterNormal = calculateWaterNormal(worldPos.xz, elapsedTime);
        
        // Add fine surface detail
        waterNormal = addSurfaceDetail(waterNormal, worldPos.xz, elapsedTime);
        
        // Calculate view direction
        vec3 viewDir = normalize(-pos);
        
        // Calculate reflection direction using animated normal
        vec3 reflectionDir = reflect(-viewDir, waterNormal);
        
        // Sample sky reflection
        vec3 rayStart = vec3(0.0, 1800.0, 0.0);
        vec3 skyboxReflection = GetSkyColor(rayStart, reflectionDir, INFINITY, normalize(lightDirection), vec3(1.0));

        // Calculate enhanced Fresnel effect
        float fresnel = calculateFresnel(viewDir, waterNormal);
        
        // Calculate water base color with depth and lighting
        float depth = gl_FragCoord.z; // Use fragment depth
        vec3 waterColor = calculateWaterColor(viewDir, waterNormal, depth);
        
        // Add some light scattering based on surface normal
        float lightScatter = max(0.0, dot(waterNormal, lightDirection));
        waterColor += lightColor * lightScatter * 0.2;
        
        // Create caustic-like effect using animated noise
        vec2 causticUV = worldPos.xz * 0.1 + vec2(elapsedTime * 0.02);
        float caustic = noise(causticUV) * noise(causticUV * 2.1) * 0.3;
        waterColor += vec3(caustic * 0.1, caustic * 0.2, caustic * 0.15);
        
        // Mix water color with skybox reflection based on Fresnel
        vec3 finalColor = mix(waterColor, skyboxReflection, fresnel);
        
        // Add some additional blue tint to maintain water appearance
        finalColor = mix(finalColor, vec3(0.2, 0.4, 0.8), 0.1);
        
        // Apply original color tinting
        finalColor *= color;
        
        // Output water with animated normal and proper transparency
        gAlbedo = vec4(finalColor, 0.85);
        gNormal = vec4(waterNormal, 0.0);
        gPosition = vec4(pos, gl_FragCoord.z);
        return;
    }
    
    // Handle non-water blocks (original code)
    vec3 blinnPhong = blinnPhongDirectional(lightDirection, lightColor, pos, normal, objectColor, shininess);
    tex.rgb *= color;
    tex.rgb *= blinnPhong;
    
    gAlbedo = tex;
    gNormal = vec4(normal, 0.0);
    gPosition = vec4(pos, gl_FragCoord.z);
}