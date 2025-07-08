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