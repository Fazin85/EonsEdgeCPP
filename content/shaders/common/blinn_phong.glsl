vec3 blinnPhongDirectional(vec3 lightDir, vec3 lightColor, vec3 fragPos, 
                          vec3 normal, vec3 albedo, float shininess) {
    
    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize(lightDir);
    vec3 viewDir = normalize(-fragPos);
    
    vec3 halfwayDir = normalize(lightDirection + viewDir);
    
    // Smooth transition as sun approaches horizon
    float sunVisibility = smoothstep(-0.1, 0.1, lightDirection.y);
    
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * sunVisibility;
    
    float normalization = (shininess + 8.0) / (8.0 * 3.14159265359);
    
    float fresnelTerm = pow(1.0 - max(dot(viewDir, norm), 0.0), 3.0);
    
    float spec = 0.0;
    if (diff > 0.0) {
        spec = normalization * pow(max(dot(norm, halfwayDir), 0.0), shininess);
    }
    vec3 specular = spec * lightColor * sunVisibility;
    
    vec3 result = (ambient + diffuse + specular) * albedo;
    
    return result;
}