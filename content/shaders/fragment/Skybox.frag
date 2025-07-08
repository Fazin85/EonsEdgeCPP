#version 430 core

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
    vec3 rayStart = GetPlanetPosition(vec3(0.0, cameraWorldPos.y, 0.0));
    vec3 rayDir = viewDir;
    float rayLength = INFINITY;
    vec3 lightColor = vec3(1.0);
    vec3 cloudWorldPosition = cameraWorldPos;
    cloudWorldPosition.x += elapsedTime * 10.0;
    
    vec3 color = GetSkyColor(rayStart, rayDir, rayLength, normalize(lightDirection), lightColor, cloudWorldPosition);
    FragColor = vec4(color, 1.0);
}