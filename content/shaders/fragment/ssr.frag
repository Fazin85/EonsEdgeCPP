#version 330 core

in vec2 texCoord;

layout(location = 0) out vec4 FragColor;

uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gPosition;

uniform mat4 ppProjection;
uniform mat4 ppView;
uniform mat4 ppInvProjection;
uniform mat4 ppInvView;

// SSR specific uniforms
uniform float maxDistance;
uniform int maxSteps;
uniform float thickness;
uniform float fadeDistance;
uniform float edgeFadeDistance;

vec3 viewPosFromDepth(vec2 uv, float depth) {
    vec4 clipPos = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewPos = ppInvProjection * clipPos;
    return viewPos.xyz / viewPos.w;
}

vec3 binarySearchDepthBuffer(vec3 rayOrigin, vec3 rayDir, float minT, float maxT) {
    float bestT = minT;
    bool foundIntersection = false;
    
    for (int i = 0; i < 8; i++) {
        float t = (minT + maxT) * 0.5;
        vec3 currentPos = rayOrigin + rayDir * t;
        
        vec4 screenPos = ppProjection * vec4(currentPos, 1.0);
        screenPos.xyz /= screenPos.w;
        
        if (screenPos.x < -1.0 || screenPos.x > 1.0 || 
            screenPos.y < -1.0 || screenPos.y > 1.0) {
            maxT = t;
            continue;
        }
        
        vec2 screenUV = screenPos.xy * 0.5 + 0.5;
        vec4 scenePos = texture(gPosition, screenUV);
        float sceneDepth = scenePos.w;
        
        float depthDiff = screenPos.z - sceneDepth;
        
        if (depthDiff > 0.0 && depthDiff < thickness) {
            // We're behind the surface but within thickness - this is a hit
            bestT = t;
            foundIntersection = true;
            maxT = t; // Search for closer intersection
        } else if (depthDiff > thickness) {
            // Too far behind surface
            maxT = t;
        } else {
            // In front of surface
            minT = t;
        }
    }
    
    if (!foundIntersection) {
        return vec3(-1.0); // Invalid result
    }
    
    vec3 finalPos = rayOrigin + rayDir * bestT;
    vec4 screenPos = ppProjection * vec4(finalPos, 1.0);
    screenPos.xyz /= screenPos.w;
    
    return vec3(screenPos.xy * 0.5 + 0.5, screenPos.z);
}

vec3 efficientRayMarch(vec3 rayOrigin, vec3 rayDir, out bool hit, out float confidence) {
    hit = false;
    confidence = 0.0;
    
    float stepSize = maxDistance / float(maxSteps);
    vec3 currentPos = rayOrigin;
    float prevDepthDiff = -thickness - 1.0; // Initialize to definitely "in front"
    vec3 prevPos = rayOrigin;
    
    for (int i = 1; i <= maxSteps; i++) {
        currentPos = rayOrigin + rayDir * (stepSize * float(i));
        
        // Transform to screen space
        vec4 screenPos = ppProjection * vec4(currentPos, 1.0);
        screenPos.xyz /= screenPos.w;
        
        // Check bounds
        if (screenPos.x < -1.0 || screenPos.x > 1.0 || 
            screenPos.y < -1.0 || screenPos.y > 1.0 ||
            screenPos.z < 0.0 || screenPos.z > 1.0) {
            break;
        }
        
        vec2 screenUV = screenPos.xy * 0.5 + 0.5;
        vec4 scenePos = texture(gPosition, screenUV);
        float sceneDepth = scenePos.w;
        
        float depthDiff = screenPos.z - sceneDepth;
        
        // Check for intersection: we crossed from in front to behind surface
        if (depthDiff > 0.0 && depthDiff < thickness && prevDepthDiff < 0.0) {
            // Found intersection! Do binary search refinement
            vec3 result = binarySearchDepthBuffer(rayOrigin, rayDir, 
                stepSize * float(i - 1), stepSize * float(i));
            
            // Check if binary search found a valid result
            if (result.x >= 0.0) {
                // Calculate confidence based on edge distance and ray distance
                vec2 edgeDistance = min(result.xy, 1.0 - result.xy);
                float edgeFade = smoothstep(0.0, edgeFadeDistance, min(edgeDistance.x, edgeDistance.y));
                float distanceFade = 1.0 - smoothstep(0.0, fadeDistance, stepSize * float(i));
                
                confidence = edgeFade * distanceFade;
                hit = true;
                return result;
            }
        }
        
        prevDepthDiff = depthDiff;
        prevPos = currentPos;
        
        // Early termination if we're getting too far behind surfaces
        if (depthDiff > thickness * 3.0) {
            break;
        }
    }
    
    return vec3(0.0);
}

void main() {
    vec4 albedo = texture(gAlbedo, texCoord);
    vec4 normalData = texture(gNormal, texCoord);
    vec4 positionData = texture(gPosition, texCoord);
    
    vec3 worldPos = positionData.xyz;
    vec3 normal = normalize(normalData.xyz);
    float depth = positionData.w;
    
    // Convert world position to view space
    vec3 viewPos = (ppView * vec4(worldPos, 1.0)).xyz;
    vec3 viewNormal = normalize((ppView * vec4(normal, 0.0)).xyz);
    
    // Calculate view direction and reflection
    vec3 viewDir = normalize(viewPos);
    vec3 reflectionDir = reflect(viewDir, viewNormal);
    
    // Early exit if reflection points away from camera
    if (reflectionDir.z > 0.0) {
        FragColor = albedo;
        return;
    }
    
    // Perform ray marching
    bool hit;
    float confidence;
    vec3 hitPoint = efficientRayMarch(viewPos, reflectionDir, hit, confidence);
    
    if (hit && confidence > 0.0) {
        // Sample the reflected color
        vec3 reflectedColor = texture(gAlbedo, hitPoint.xy).rgb;
        
        // Apply fresnel effect - but make it more aggressive for water
        float NdotV = max(dot(-viewDir, viewNormal), 0.0);
        float fresnel = pow(1.0 - NdotV, 5.0);
        
        float reflectivity = normalData.w;

        // For water, use much stronger fresnel and lower base reflectance
        if (reflectivity > 0.9) { // Assume this is water
            fresnel = mix(0.02, 1.0, fresnel); // Very low base reflectance
            fresnel = pow(fresnel, 0.5); // Make fresnel curve more aggressive
            
            // For water, almost completely replace the base color at grazing angles
            float waterReflectionStrength = mix(0.1, 1.0, fresnel);
            vec3 finalColor = mix(albedo.rgb, reflectedColor, reflectivity * confidence * waterReflectionStrength);
            FragColor = vec4(finalColor, 1.0);
        } else {
            // Regular materials
            fresnel = mix(0.04, 1.0, fresnel);
            vec3 finalColor = mix(albedo.rgb, reflectedColor, fresnel * confidence * reflectivity);
            FragColor = vec4(finalColor, 1.0);
        }
    } else {
        // No reflection, just return albedo
        FragColor = albedo;
    }
}