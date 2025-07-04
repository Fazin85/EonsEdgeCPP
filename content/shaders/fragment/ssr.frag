#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gPosition;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 invProjection;
uniform mat4 invView;

uniform float maxDistance;
uniform int maxSteps;
uniform float thickness;

// Convert view space coordinates to screen space
vec2 viewToScreen(vec3 viewPos) {
    vec4 clipSpace = projection * vec4(viewPos, 1.0);
    vec3 ndcSpace = clipSpace.xyz / clipSpace.w;
    return ndcSpace.xy * 0.5 + 0.5;
}

// Ray marching function
vec4 rayMarch(vec3 rayOrigin, vec3 rayDirection) {
    float stepSize = maxDistance / float(maxSteps);
    vec3 currentPos = rayOrigin;
    
    for (int i = 0; i < maxSteps; i++) {
        currentPos += rayDirection * stepSize;
        
        // Convert to screen space
        vec2 screenPos = viewToScreen(currentPos);
        
        // Check if we're outside screen bounds
        if (screenPos.x < 0.0 || screenPos.x > 1.0 || 
            screenPos.y < 0.0 || screenPos.y > 1.0) {
            break;
        }
        
        // Sample world position and depth at current position
        vec4 sampledData = texture(gPosition, screenPos);
        vec3 sampledWorldPos = sampledData.xyz;
        
        // Check if we hit something by comparing Z coordinates
        // Since we're using camera-relative rendering, Z comparison should work directly
        float depthDifference = currentPos.z - sampledWorldPos.z;
        
        if (depthDifference > 0.0 && depthDifference < thickness) {
            // We hit something, return the color at this position
            vec3 hitColor = texture(gAlbedo, screenPos).rgb;
            
            // Calculate fade based on distance and screen edge
            float fadeDistance = length(currentPos - rayOrigin) / maxDistance;
            vec2 screenFade = vec2(
                min(screenPos.x, 1.0 - screenPos.x),
                min(screenPos.y, 1.0 - screenPos.y)
            );
            float edgeFade = min(screenFade.x, screenFade.y) * 10.0;
            
            float totalFade = (1.0 - fadeDistance) * clamp(edgeFade, 0.0, 1.0);
            
            return vec4(hitColor, totalFade);
        }
    }
    
    return vec4(0.0, 0.0, 0.0, 0.0);
}

void main() 
{
    vec3 albedo = texture(gAlbedo, texCoord).rgb;
    vec4 depthData = texture(gPosition, texCoord);
    vec3 worldPos = depthData.xyz;
    float depth = depthData.w;
    vec3 normal = texture(gNormal, texCoord).rgb;
    
    if(depth > 0.999) 
    {
        FragColor = vec4(albedo, 1.0);
        return;
    }
    
    // worldPos is essentially viewPos (camera at origin)
    vec3 viewPos = worldPos;
    
    // Decode normal from [0,1] to [-1,1] range
    vec3 viewNormal = normalize(normal * 2.0 - 1.0);
    
    vec3 viewDir = normalize(viewPos);
    vec3 reflectionDir = reflect(viewDir, viewNormal);
    
    vec4 reflectionResult = rayMarch(viewPos, reflectionDir);
    
    vec3 finalColor = mix(albedo, reflectionResult.rgb, reflectionResult.a * 0.5);
    FragColor = vec4(finalColor, 1.0);
}