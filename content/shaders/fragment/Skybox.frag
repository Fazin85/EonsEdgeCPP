#version 430 core

//this is from https://www.shadertoy.com/view/lcfSRl

out vec4 FragColor;

in vec3 pos;
in vec3 texCoords;

uniform float elapsedTime;
uniform vec3 cameraWorldPos;

#include "../common/sky.glsl"
#include "../common/clouds.glsl"

uniform vec3 lightDirection;

void main()
{    
    vec3 viewDir = normalize(pos);
    
    // Set up atmosphere parameters
    vec3 rayStart = vec3(0.0, 1800.0, 0.0);
    vec3 rayDir = viewDir;
    float rayLength = INFINITY;
    vec3 lightColor = vec3(1.0);
    
    vec3 color = GetSkyColor(rayStart, rayDir, rayLength, normalize(lightDirection), lightColor);
    FragColor = vec4(color, 1.0);
}