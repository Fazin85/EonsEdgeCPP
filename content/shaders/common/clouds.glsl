layout(binding = 15) uniform sampler3D TEX_CLOUD_NOISE;

const float CLOUD_LAYER_1_HEIGHT = 1000.0;
const float CLOUD_LAYER_2_HEIGHT = 2000.0;
const float CLOUD_LAYER_3_HEIGHT = 2000.0;
const float CLOUD_THICKNESS = 500.0;
const float CLOUD_DENSITY_SCALE = 0.5;
const float CLOUD_SPEED = 0.02;
const float CLOUD_SCALE = 0.00025;

float GetCloudDensity(const in vec2 pos, const in float time) {
    vec3 uv = vec3(pos, time);
    float f = 0.0;
    float weight = 0.7;
    float maxWeight = 1e-7;
    
    for (int i = 0; i < 6; i++) {
        vec2 noise = texture(TEX_CLOUD_NOISE, uv).rg;
        f += weight * noise.r;
        uv.xy *= 2.0;
        maxWeight += weight;
        weight *= 0.6;
    }
    f /= maxWeight;
    
    // Add simple large-scale variation
    float variation = texture(TEX_CLOUD_NOISE, vec3(pos * 0.0001, time * 0.02)).r;
    f *= mix(0.8, 1.2, variation);
    
    f = smoothstep(0.42, 1.0, f);
    f = pow(f, 0.6);
    return f;
}

// Sample cloud density at a given world position
float sampleCloudDensity(vec3 worldPos, float layerHeight) {
    // Project world position to 2D cloud plane
    vec2 cloudUV = worldPos.xz * CLOUD_SCALE;
    
    // Animate clouds
    float animTime = elapsedTime * CLOUD_SPEED;
    
    // Sample using your existing function
    float cloudSample = GetCloudDensity(cloudUV, animTime);
    
    // Create vertical falloff for cloud thickness
    float heightFactor = 1.0 - abs(worldPos.y - layerHeight) / CLOUD_THICKNESS;
    heightFactor = smoothstep(0.0, 1.0, heightFactor);
    
    return cloudSample * heightFactor * CLOUD_DENSITY_SCALE;
}

vec4 getCloudColor(vec3 rayStart, vec3 rayDir, float rayLength, vec3 lightDir, vec3 lightColor, vec3 originalColor) {
    vec3 cloudColor = vec3(0.0);
    float cloudTransmittance = 1.0;
    
    // Define cloud layers
    float cloudLayers[2] = float[2](CLOUD_LAYER_1_HEIGHT, CLOUD_LAYER_2_HEIGHT);
    
    for (int layer = 0; layer < 2; layer++) {
        float layerHeight = cloudLayers[layer];
        
        // Calculate intersection with cloud layer plane
        if (abs(rayDir.y) < 0.001) continue; // Ray nearly parallel to cloud layer
        
        float t = (layerHeight - rayStart.y) / rayDir.y;
        
        // Check if intersection is within ray bounds and in front of camera
        if (t > 0.0 && t < rayLength && t < 10000) {
            vec3 hitPoint = rayStart + rayDir * t;
            
            // Sample cloud density at intersection point
            float density = sampleCloudDensity(hitPoint, layerHeight);
            
            if (density > 0.0) {
                // === ATMOSPHERE SAMPLING SECTION ===
                
                // Sample atmosphere from cloud position to light source
                vec4 atmosphereTransmittance;
                
                // Get atmospheric scattering for the cloud illumination
                vec3 atmosphereScattering = GetAtmosphere(
                    hitPoint,           // Cloud position as ray start
                    lightDir,           // Direction toward light
                    INFINITY,           // Distance to sample
                    lightDir,           // Light direction
                    lightColor,         // Light color
                    atmosphereTransmittance
                );
                
                // Sample atmosphere from camera to cloud for aerial perspective
                vec4 aerialTransmittance;
                vec3 aerialScattering = GetAtmosphere(
                    rayStart,           // Camera position
                    rayDir,             // View direction
                    t,                  // Distance to cloud
                    lightDir,           // Light direction
                    lightColor,         // Light color
                    aerialTransmittance
                );
                
                // === CLOUD LIGHTING CALCULATION ===
                
                // Use atmosphere transmittance to modulate light reaching the cloud
                vec3 effectiveLightColor = lightColor * atmosphereTransmittance.xyz;
                
                // Basic cloud lighting
                float lightDot = max(0.0, dot(lightDir, vec3(0, 1, 0)));
                //float backLight = max(0.0, dot(-lightDir, rayDir)) * 0.05; // Back-scattering
                float backLight = 0.0;

                // Cloud base color influenced by atmospheric scattering
                vec3 baseCloudColor = vec3(0.9, 0.95, 1.0);
                
                // Primary illumination with atmospheric influence
                vec3 sunColor = effectiveLightColor * lightDot;
                
                // Ambient lighting from atmospheric scattering
                vec3 ambientColor = atmosphereScattering * 0.3;
                
                // Back-scattering with atmospheric color
                vec3 backScatter = effectiveLightColor * backLight * 0.3;
                
                // Combine lighting components
                vec3 layerColor = baseCloudColor * (ambientColor + sunColor * 0.8 + backScatter);
                
                // Apply density-based opacity
                float alpha = density * (1.0 - exp(-density * 2.0));
                alpha = clamp(alpha, 0.0, 1.0);
                
                // Apply aerial perspective transmittance to the cloud color
                layerColor *= aerialTransmittance.xyz;
                
                // Blend with existing clouds (front to back)
                cloudColor = cloudColor + layerColor * alpha * cloudTransmittance;

                cloudTransmittance *= (1.0 - alpha);
                
                // Early exit if clouds are opaque
                if (cloudTransmittance < 0.01) break;
            }
        }
    }
    
    return vec4(cloudColor, cloudTransmittance);
}