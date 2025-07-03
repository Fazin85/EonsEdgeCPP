vec3 blinnPhongDirectional(vec3 lightDir, vec3 lightColor, vec3 viewPos, vec3 fragPos, 
                          vec3 normal, vec3 albedo, float shininess) {
    
    // Normalize vectors
    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize(-lightDir); // Direction TO light
    vec3 viewDir = normalize(viewPos - fragPos);
    
    // Calculate halfway vector (Blinn-Phong modification)
    vec3 halfwayDir = normalize(lightDirection + viewDir);
    
    // Ambient component
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse component (Lambert's cosine law)
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Energy conservation for specular
    float normalization = (shininess + 8.0) / (8.0 * 3.14159265359);
    
    // Fresnel effect
    float fresnelTerm = pow(1.0 - max(dot(viewDir, norm), 0.0), 3.0);
    
    // Specular component (Enhanced Blinn-Phong)
    float spec = normalization * pow(max(dot(norm, halfwayDir), 0.0), shininess) * diff;
    vec3 specular = spec * lightColor * (1.0 + fresnelTerm * 0.5);
    
    // Combine all components
    vec3 result = (ambient + diffuse + specular) * albedo;
    
    return result;
}