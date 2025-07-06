#version 330 core

//this is from https://www.shadertoy.com/view/lcfSRl

#include "../common/sky.glsl"

out vec4 FragColor;

in vec3 pos;
in vec3 texCoords;

uniform samplerCube skybox;

vec3 TonemapACES(vec3 color)
{
    return (color * (2.51 * color + 0.03)) / (color * (2.43 * color + 0.59) + 0.14);
}

const vec3 HEIGHT_FOG_COLOR = vec3(0.8, 0.9, 1.0);
const float HEIGHT_FOG_PLANE = 1000.0;
const float HEIGHT_FOG_DENSITY = 5e-5;
const float CAM_EXPOSURE = 20.0;
const float CAM_GAMMA = 4.0;
const float CAM_AUTO_EXPOSURE_EV_LIMIT = 4.0;

float GetFogWeight(vec3 ro, vec3 rd, float rl, float h)
{
    vec2 fogt = SphereIntersection(ro, rd, PLANET_CENTER, PLANET_RADIUS + HEIGHT_FOG_PLANE);
    fogt.x = max(0.0, fogt.x);
    fogt.y = min(fogt.y, rl);
    int sc = 4;
    float ss = (fogt.y - fogt.x) / float(sc);
    float od = 0.0;
    for (int i = 0; i < sc; i++)
    {
        float delta = (float(i) + 0.5) / float(sc);
        vec3 p = ro + rd * mix(fogt.x, fogt.y, delta);
        float y = distance(p, PLANET_CENTER) - PLANET_RADIUS;
        float f = max(0.0, sq(1.0 - y / HEIGHT_FOG_PLANE));
        od += f * ss;
    }
    return 1.0 - exp(-od * HEIGHT_FOG_DENSITY);
}

void main()
{    
    vec3 viewDir = normalize(texCoords);
    
    // Set up atmosphere parameters
    vec3 rayStart = vec3(0.0, 1800.0, 0.0);
    vec3 rayDir = viewDir;
    float rayLength = INFINITY; // Very far for skybox
    vec3 lightDir = normalize(vec3(0.5, 0.5, 0.5));
    vec3 lightColor = vec3(1.0);
    
    vec3 color = vec3(0.0);

    vec4 transmittance;
    float altitude = max(0.0, (rayStart + rayDir * rayLength).y);
    float fogWeight = GetFogWeight(rayStart, rayDir, rayLength, altitude);
    vec3 scattering = GetAtmosphere(rayStart, rayDir, rayLength, lightDir, lightColor, transmittance, vec4(HEIGHT_FOG_COLOR, fogWeight), 1.0);
    color = color * transmittance.xyz + scattering;

    vec3 radiance = GetAtmosphere(rayStart, vec3(0, 1, 0), INFINITY, lightDir, lightColor);
    float lum = dot(radiance, vec3(0.3, 0.59, 0.11));
    color *= min(CAM_AUTO_EXPOSURE_EV_LIMIT, 0.003 / clamp(lum, 0.0002, 1.0));

    color *= CAM_EXPOSURE;

    color *= TonemapACES(color);

    color = pow(color, vec3(1.0 / CAM_GAMMA));

    FragColor = vec4(color, 1.0);
}