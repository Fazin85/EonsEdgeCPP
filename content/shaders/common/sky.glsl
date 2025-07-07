//this is from https://www.shadertoy.com/view/lcfSRl

// Copyright (c) 2024 Felix Westin
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

/////////////////////////////////////////////////////////////////////////

// Fast semi-physical atmosphere with planet view and aerial perspective.
//
// I have long dreamed of (and tried making) a function that
// generates plausible atmospheric scattering and transmittance without
// expensive ray marching that also supports aerial perspectives and
// offers simple controls over perceived atmospheric density which do not
// affect the color of the output.
//
// This file represents my latest efforts in making such a function and
// this time I am happy enough with the result to release it.
//
// Big thanks to:
// Inigo Quilez (https://iquilezles.org) for this site and his great
// library of shader resources.
// Sébastien Hillaire (https://sebh.github.io) for his many papers on
// atmospheric and volumetric rendering.

/////////////////////////////////////////////////////////////////////////

// Config
// Draw planet ground sphere.
//#define DRAW_PLANET
// Force camera to stay above horizon. Useful for certain games.
//#define PREVENT_CAMERA_GROUND_CLIP
// Comment out if light color is not in radiometric units.
//#define LIGHT_COLOR_IS_RADIANCE
const bool DRAW_PLANET = true;
const bool PREVENT_CAMERA_GROUND_CLIP = true;
const bool LIGHT_COLOR_IS_RADIANCE = true;
const float AERIAL_SCALE           =    3.0; // Higher value = more aerial perspective. A value of 1 is tuned to match reference implementation.
const float NIGHT_LIGHT            =    2e-3; // Optional, cheap (free) non-physical night lighting. Makes twilight a bit purple which can look nice.
const float SUN_DISC_SIZE         =     1.0; // 1 is physical sun size (0.5 degrees).

// Math
const float INFINITY = 3.402823466e38;
const float PI       = 3.14159265359;

// Atmosphere parameters (physical)
const float ATMOSPHERE_HEIGHT = 100000.0;
const float ATMOSPHERE_DENSITY = 1.0;
const float PLANET_RADIUS      = 6371000.0;
const vec3 PLANET_CENTER      = vec3(0, -PLANET_RADIUS, 0);
const vec3 C_RAYLEIGH         = (vec3(5.802, 13.558, 33.100) * 1e-6);
const vec3 C_MIE              = (vec3(3.996, 3.996, 3.996) * 1e-6);
const vec3 C_OZONE           =  (vec3(0.650, 1.881, 0.085) * 1e-6);

// Atmosphere parameters (approximation)
const float  RAYLEIGH_MAX_LUM  =  2.5;
const float  MIE_MAX_LUM       =  0.5;

// Magic numbers
const float M_EXPOSURE_MUL      =   0.23; // Tuned to match physical reference.
const float M_FAKE_MS           =   0.3; // Physical multiple scattering results in ~30% increase in energy.
const float M_AERIAL            =   2.5;
const float M_TRANSMITTANCE     =   0.25;
const float M_LIGHT_TRANSMITTANCE = 1e6;
const float M_MIN_LIGHT_ELEVATION = -0.3;
const float M_DENSITY_HEIGHT_MOD =  1e-12;
const float  M_DENSITY_CAM_MOD    =  10.0;
const float M_OZONE              =  1.5;
const float M_OZONE2             =  5.0;
const vec3 M_MIE                =  vec3(0.95, 0.85, 0.75);

float sq(float x) { return x*x; }
float pow4(float x) { return sq(x)*sq(x); }
float pow8(float x) { return pow4(x)*pow4(x); }
float saturate(float x) { return clamp(x, 0., 1.); }

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

// https://iquilezles.org/articles/intersectors/
vec2 SphereIntersection(vec3 rayStart, vec3 rayDir, vec3 sphereCenter, float sphereRadius)
{
	vec3 oc = rayStart - sphereCenter;
    float b = dot(oc, rayDir);
    float c = dot(oc, oc) - sq(sphereRadius);
    float h = sq(b) - c;
    if (h < 0.0)
    {
        return vec2(-1.0, -1.0);
    }
    else
    {
        h = sqrt(h);
        return vec2(-b-h, -b+h);
    }
}

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

vec2 PlanetIntersection(vec3 rayStart, vec3 rayDir)
{
	return SphereIntersection(rayStart, rayDir, PLANET_CENTER, PLANET_RADIUS);
}
vec2 AtmosphereIntersection(vec3 rayStart, vec3 rayDir)
{
	return SphereIntersection(rayStart, rayDir, PLANET_CENTER, PLANET_RADIUS + ATMOSPHERE_HEIGHT);
}

float PhaseR(float costh)
{
	return (1.0+sq(costh))*0.06;
}
float PhaseM(float costh, float g)
{
	g = min(g, 0.9381);
	float k = 1.55*g-0.55*sq(g)*g;
	float a = 1.0-sq(k);
	float b = 12.57*sq(1.0-k*costh);
	return a/b;
}

vec3 GetLightTransmittance(vec3 position, vec3 lightDir, float multiplier, float ozoneMultiplier)
{
    float lightExtinctionAmount = exp(-(saturate(lightDir.y + 0.05) * 40.0)) +
        exp(-(saturate(lightDir.y + 0.5) * 5.0)) * 0.4 +
        sq(saturate(1.0-lightDir.y)) * 0.02 +
        0.002;
	return exp(-(C_RAYLEIGH + C_MIE + C_OZONE * ozoneMultiplier) * lightExtinctionAmount * ATMOSPHERE_DENSITY * multiplier * M_LIGHT_TRANSMITTANCE);
}
vec3 GetLightTransmittance(vec3 position, vec3 lightDir)
{
	return GetLightTransmittance(position, lightDir, 1.0, 1.0);
}

void GetRayleighMie(float opticalDepth, float densityR, float densityM, out vec3 R, out vec3 M)
{
    // Approximate marched Rayleigh + Mie scattering with some exp magic.
    R = (1.0 - exp(-opticalDepth * densityR * C_RAYLEIGH / RAYLEIGH_MAX_LUM)) * RAYLEIGH_MAX_LUM;
	M = (1.0 - exp(-opticalDepth * densityM * C_MIE / MIE_MAX_LUM)) * MIE_MAX_LUM;
}

// Main atmosphere function
vec3 GetAtmosphere(
    vec3 rayStart,      // Camera position
    vec3 rayDir,        // View direction
    float  rayLength,     // View distance
    vec3 lightDir,      // Light (sun) direction
	vec3 lightColor,    // Light (sun) color. Usually white
out vec4 transmittance, // Atmospheric transmittance in xyz, planet intersection flag in w
    vec4 fogFactor,     // (Optional) Fog "fade" factor. Can be used to add your own height fog or to fade the world out
    float  occlusion      // (Optional) Scattering occlusion (god rays)
) {
/*#ifdef PREVENT_CAMERA_GROUND_CLIP
	rayStart.y = max(rayStart.y, 1.0);
#endif*/

    if(PREVENT_CAMERA_GROUND_CLIP) {
        rayStart.y = max(rayStart.y, 1.0);
    }

	// Planet and atmosphere intersection to get optical depth
	// TODO: Could simplify to circle intersection test if flat horizon is acceptable
	vec2 t1 = PlanetIntersection(rayStart, rayDir);
	vec2 t2 = AtmosphereIntersection(rayStart, rayDir);
    
    // Note: This only works if camera XZ is at 0. Otherwise, swap for the line below.
    float altitude = rayStart.y;
    //float altitude = (length(rayStart - PLANET_CENTER) - PLANET_RADIUS);
    float normAltitude = rayStart.y / ATMOSPHERE_HEIGHT;

	if (t2.y < 0.0)
	{
		// Outside of atmosphere looking into space, return nothing
		transmittance = vec4(1, 1, 1, 1);
		return vec3(0, 0, 0);
	}
    else
    {
        // In case camera is outside of atmosphere, subtract distance to entry.
        t2.y -= max(0.0, t2.x);

        float opticalDepth = DRAW_PLANET ? t1.x > 0.0 ? min(t1.x, t2.y) : t2.y : t2.y;

        // Optical depth modulators
        opticalDepth = min(rayLength, opticalDepth);
        opticalDepth = min(opticalDepth * M_AERIAL * AERIAL_SCALE, t2.y);

        // Altitude-based density modulators
        float hbias = 1.0-1.0/(2.0+sq(t2.y)*M_DENSITY_HEIGHT_MOD);
        hbias = pow(hbias, 1.0+normAltitude*M_DENSITY_CAM_MOD); // Really need a pow here, bleh
        float sqhbias = sq(hbias);
        float densityR = sqhbias * ATMOSPHERE_DENSITY;
        float densityM = sq(sqhbias)*hbias * ATMOSPHERE_DENSITY;

        // Apply light transmittance (makes sky red as sun approaches horizon)
        float ly = lightDir.y;
        ly += saturate(-lightDir.y + 0.02) * saturate(lightDir.y + 0.7);
        ly = clamp(ly, -1.0, 1.0);
        lightColor *= GetLightTransmittance(rayStart, vec3(lightDir.x, ly, lightDir.z), hbias, M_OZONE2);

        if(LIGHT_COLOR_IS_RADIANCE) {
            lightColor *= PI;
        }

        vec3 R, M;
        GetRayleighMie(opticalDepth, densityR, densityM, R, M);
        
        vec3 E = (C_RAYLEIGH * densityR + C_MIE * densityM + C_OZONE * densityR * M_OZONE) * pow4(1.0 - normAltitude) * M_TRANSMITTANCE;

        float costh = dot(rayDir, lightDir);
        float phaseR = PhaseR(costh);
        float phaseM = PhaseM(costh, 0.88);
        
        float nightLight = NIGHT_LIGHT;

        // Combined scattering
        vec3 rayleigh = (phaseR * occlusion + phaseR * M_FAKE_MS) * lightColor + nightLight * phaseR;
        vec3 mie = ((phaseM * occlusion + phaseR * M_FAKE_MS) * lightColor + nightLight * phaseR) * M_MIE;
        vec3 scattering = mie * M + rayleigh * R;

        // View extinction, matched to reference
        transmittance.xyz = exp(-(opticalDepth + pow8(opticalDepth * 4.5e-6)) * E);
        // Store planet intersection flag in transmittance.w, useful for occluding clouds, celestial bodies etc.
        transmittance.w = step(t1.x, 0.0);

        if (fogFactor.w > 0.0)
        {
            // 2nd sample (all the way to atmosphere exit), used for fog fade.
            opticalDepth = t2.y;
            GetRayleighMie(opticalDepth, densityR, densityM, R, M);
            vec3 scattering2 = mie * M + rayleigh * R;
            vec3 transmittance2 = exp(-opticalDepth * E);

            scattering2 *= mix(fogFactor.xyz, vec3(1, 1, 1), sq(fogFactor.w)); // Fog color test
            scattering = mix(scattering, scattering2, fogFactor.w);
            transmittance.xyz = mix(transmittance.xyz, transmittance2, fogFactor.w);
        }
        
        if (t1.y > 0.0 && t1.y < rayLength)
        {
            // Darken planet
            vec3 planetColor = vec3(0.4, 0.4, 0.4);
            float planetOpticalDepth = t1.y - max(0.0, t1.x);
            float skyWeight = exp(-planetOpticalDepth * 1e-6);
            scattering *= mix(planetColor, vec3(1, 1, 1), skyWeight);
        }

        return scattering * M_EXPOSURE_MUL;
    }
}

// Overloaded functions
vec3 GetAtmosphere(
    vec3 rayStart,
    vec3 rayDir,
    float rayLength,
    vec3 lightDir,
	vec3 lightColor,
out vec4 transmittance
) {
    return GetAtmosphere(rayStart, rayDir, rayLength, lightDir, lightColor, transmittance, vec4(0.0), 1.0);
}
vec3 GetAtmosphere(
    vec3 rayStart,
    vec3 rayDir,
    float rayLength,
    vec3 lightDir,
    vec3 lightColor
) {
    vec4 transmittance;
    return GetAtmosphere(rayStart, rayDir, rayLength, lightDir, lightColor, transmittance, vec4(0.0), 1.0);
}

vec3 GetSunDisc(vec3 rayDir, vec3 lightDir)
{
    const float A = cos(0.00436 * SUN_DISC_SIZE);
	float costh = dot(rayDir, lightDir);
	float disc = sqrt(smoothstep(A, 1.0, costh));
	return vec3(disc, disc, disc);
}

vec3 hash32(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz+33.33);
    return fract((p3.xxy+p3.yzz)*p3.zyx);
}

vec4 getCloudColor(vec3 rayStart, vec3 rayDir, float rayLength, vec3 lightDir, vec3 lightColor, vec3 originalColor);

vec3 CalculateStars(vec3 rayDir, vec3 color, vec4 transmittance)
{
    vec3 starWorldPos = rayDir * 1000.0; // Project ray to distant sphere
    vec2 starCellF = starWorldPos.xz / 4.0; // Use XZ plane for star distribution

    // Handle negative coordinates by adding a large offset before flooring
    vec2 offsetCoords = starCellF + vec2(10000.0); // Large offset to ensure positive
    uvec2 starCell = uvec2(floor(offsetCoords));
    starCellF = fract(starCellF); // Keep original fractional part

    vec3 starHash = hash32(vec2(starCell));

    // Only show stars when looking up (prevent stars on ground)
    float starVisibility = smoothstep(0.0, 0.2, rayDir.y);
    vec3 stars = smoothstep(0.1, 0.0, length(starCellF - starHash.xy)) * vec3(pow(starHash.z, 10.0)) * 5e-3;
    color += stars * transmittance.w * starVisibility;
    return color;
}

vec3 GetSkyColor(vec3 rayStart, vec3 rayDir, float rayLength, vec3 lightDir, vec3 lightColor) 
{
    vec3 color = vec3(0.0);

    vec4 transmittance;
    float altitude = max(0.0, (rayStart + rayDir * rayLength).y);
    float fogWeight = GetFogWeight(rayStart, rayDir, rayLength, altitude);
    vec3 scattering = GetAtmosphere(rayStart, rayDir, rayLength, lightDir, lightColor, transmittance, vec4(HEIGHT_FOG_COLOR, fogWeight), 1.0);
    color = color * transmittance.xyz + scattering;

    // Stars
    color += CalculateStars(rayDir, color, transmittance);

    vec3 cloudWorldPosition = cameraWorldPos;
    cloudWorldPosition.x += elapsedTime * 10;

    vec4 cloudResult = getCloudColor(cloudWorldPosition, rayDir, rayLength, lightDir, lightColor, color);
    
    // Blend clouds with atmosphere
    color = color * cloudResult.w + cloudResult.xyz;

    //auto exposure
    vec3 radiance = GetAtmosphere(rayStart, vec3(0, 1, 0), INFINITY, lightDir, lightColor);
    float lum = dot(radiance, vec3(0.3, 0.59, 0.11));
    color *= min(CAM_AUTO_EXPOSURE_EV_LIMIT, 0.003 / clamp(lum, 0.0002, 1.0));

    //exposure
    color *= CAM_EXPOSURE;

    //tonemaping
    color *= TonemapACES(color);

    //gamma correction
    color = pow(color, vec3(1.0 / CAM_GAMMA));
    
    return color;
}