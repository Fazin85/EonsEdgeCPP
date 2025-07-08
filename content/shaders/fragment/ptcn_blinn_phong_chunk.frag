#version 430 core

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gPosition; // w is depth

uniform vec3 cameraWorldPos;
uniform float elapsedTime;

layout(binding = 14) uniform sampler2D noisetex;

#include "../common/sky.glsl"
#include "../common/waves.glsl"
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

mat3 getTBN() {
    vec3 N = normalize(normal);
    
    // For axis-aligned blocks, use consistent tangent space
    vec3 T = vec3(1, 0, 0);
    vec3 B = vec3(0, 1, 0);
    
    // Adjust based on face orientation
    if (abs(N.x) > 0.9) {
        T = vec3(0, 0, 1);
        B = vec3(0, 1, 0);
    } else if (abs(N.z) > 0.9) {
        T = vec3(1, 0, 0);  
        B = vec3(0, 1, 0);
    }
    
    // Ensure proper handedness
    if (dot(cross(T, B), N) < 0.0) {
        T = -T;
    }
    
    return mat3(T, B, N);
}

vec3 GetWaterSurface(vec3 worldPos, vec3 viewDir, vec3 lightDir, vec3 lightColor, vec3 cameraPos) {
    //vec3 worldPos, vec3 tangentViewDir, float viewDist;
    //INVERT VIEWDIR?
    vec3 tangentViewDir = transpose(getTBN()) * viewDir;
    
    // Distance-based factors
    float dist = length(worldPos - cameraPos);
    float distanceFactor = 1.0 / (1.0 + dist * 0.01);

    vec3 wn = waterNormal(worldPos, tangentViewDir, dist);

    //vec3 wn = normalize(vec3(wnd.y * 1.5, 12.0, wnd.z * 1.5));
    //vec3 wn = vec3(0.0, 1.0, 0.0);
    //vec3 color = calculateWaterColorCameraDistance(worldPos - cameraPos, vec3(1.0), 1.0, 0.0);

    // Perturbed reflection
    vec3 r = reflect(viewDir, wn);
    vec2 perturbOffset = worldPos.xz * 0.1 + elapsedTime * 0.3;
    vec2 perturbNoise = Noised(perturbOffset).xy * 0.02;
    r.xz += perturbNoise * (1.0 - abs(r.y));
    r = normalize(r);
    
    // Fresnel
    float F0 = 0.02;
    float cosTheta = max(0.0, dot(-viewDir, wn));
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    fresnel = mix(0.75, 1.0, fresnel);
    
    // Get reflection color
    vec3 reflectedRayStart = GetPlanetPosition(worldPos);
    vec3 worldCloudPosition = vec3(worldPos);
    worldCloudPosition.x += elapsedTime * 10.0;
    vec3 refl = GetSkyColor(reflectedRayStart, r, INFINITY, lightDir, lightColor, worldCloudPosition);

    vec3 relativePos = worldPos - cameraPos;
    vec3 color = waterColor(relativePos + viewDir * 10.0, relativePos, viewDir, false, 0.0, refl, dot(viewDir, lightDir));
    
    return color * 2.0;
}

void main()
{
    vec4 tex = texture(albedoMap, texCoord);
    vec4 blockData = texture(blockSampler, texCoord);
    
    int blockID = int(blockData.r * 255.0);
    float shininess = blockData.g * 255.0;
    
    // water blocks
    if(blockID == 4) 
    {
        vec3 worldPos = cameraWorldPos + pos;

        vec3 viewDir = normalize(pos);
        vec3 waterColor = GetWaterSurface(worldPos, viewDir, normalize(lightDirection), vec3(1.0), cameraWorldPos);
        
        // Output water with animated normal and proper transparency
        gAlbedo = vec4(waterColor, 0.85);
        gNormal = vec4(vec3(0.0), 0.0);
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