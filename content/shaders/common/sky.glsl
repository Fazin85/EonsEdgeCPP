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

#ifndef FAST_ATMOSPHERE_INCLUDED
#define FAST_ATMOSPHERE_INCLUDED

// Lazy HLSL -> GLSL porting. Remove if you intend to use it with HLSL.
#define float2 vec2
#define float3 vec3
#define float4 vec4
#define lerp mix

// Config
#define DRAW_PLANET                // Draw planet ground sphere.
#define PREVENT_CAMERA_GROUND_CLIP // Force camera to stay above horizon. Useful for certain games.
#define LIGHT_COLOR_IS_RADIANCE    // Comment out if light color is not in radiometric units.
#define AERIAL_SCALE               3.0 // Higher value = more aerial perspective. A value of 1 is tuned to match reference implementation.
#define NIGHT_LIGHT                2e-3 // Optional, cheap (free) non-physical night lighting. Makes twilight a bit purple which can look nice.
#define SUN_DISC_SIZE              1.0 // 1 is physical sun size (0.5 degrees).

// Math
#define INFINITY 3.402823466e38
#define PI       3.14159265359

// Atmosphere parameters (physical)
#define ATMOSPHERE_HEIGHT  100000.0
#define ATMOSPHERE_DENSITY 1.0
#define PLANET_RADIUS      6371000.0
#define PLANET_CENTER      float3(0, -PLANET_RADIUS, 0)
#define C_RAYLEIGH         (float3(5.802, 13.558, 33.100) * 1e-6)
#define C_MIE              (float3(3.996, 3.996, 3.996) * 1e-6)
#define C_OZONE            (float3(0.650, 1.881, 0.085) * 1e-6)

// Atmosphere parameters (approximation)
#define RAYLEIGH_MAX_LUM   2.5
#define MIE_MAX_LUM        0.5

// Magic numbers
#define M_EXPOSURE_MUL        0.23 // Tuned to match physical reference.
#define M_FAKE_MS             0.3 // Physical multiple scattering results in ~30% increase in energy.
#define M_AERIAL              2.5
#define M_TRANSMITTANCE       0.25
#define M_LIGHT_TRANSMITTANCE 1e6
#define M_MIN_LIGHT_ELEVATION -0.3
#define M_DENSITY_HEIGHT_MOD  1e-12
#define M_DENSITY_CAM_MOD     10.0
#define M_OZONE               1.5
#define M_OZONE2              5.0
#define M_MIE                 float3(0.95, 0.85, 0.75)

float sq(float x) { return x*x; }
float pow4(float x) { return sq(x)*sq(x); }
float pow8(float x) { return pow4(x)*pow4(x); }
float saturate(float x) { return clamp(x, 0., 1.); }

// https://iquilezles.org/articles/intersectors/
float2 SphereIntersection(float3 rayStart, float3 rayDir, float3 sphereCenter, float sphereRadius)
{
	float3 oc = rayStart - sphereCenter;
    float b = dot(oc, rayDir);
    float c = dot(oc, oc) - sq(sphereRadius);
    float h = sq(b) - c;
    if (h < 0.0)
    {
        return float2(-1.0, -1.0);
    }
    else
    {
        h = sqrt(h);
        return float2(-b-h, -b+h);
    }
}
float2 PlanetIntersection(float3 rayStart, float3 rayDir)
{
	return SphereIntersection(rayStart, rayDir, PLANET_CENTER, PLANET_RADIUS);
}
float2 AtmosphereIntersection(float3 rayStart, float3 rayDir)
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

float3 GetLightTransmittance(float3 position, float3 lightDir, float multiplier, float ozoneMultiplier)
{
    float lightExtinctionAmount = exp(-(saturate(lightDir.y + 0.05) * 40.0)) +
        exp(-(saturate(lightDir.y + 0.5) * 5.0)) * 0.4 +
        sq(saturate(1.0-lightDir.y)) * 0.02 +
        0.002;
	return exp(-(C_RAYLEIGH + C_MIE + C_OZONE * ozoneMultiplier) * lightExtinctionAmount * ATMOSPHERE_DENSITY * multiplier * M_LIGHT_TRANSMITTANCE);
}
float3 GetLightTransmittance(float3 position, float3 lightDir)
{
	return GetLightTransmittance(position, lightDir, 1.0, 1.0);
}

void GetRayleighMie(float opticalDepth, float densityR, float densityM, out float3 R, out float3 M)
{
    // Approximate marched Rayleigh + Mie scattering with some exp magic.
    R = (1.0 - exp(-opticalDepth * densityR * C_RAYLEIGH / RAYLEIGH_MAX_LUM)) * RAYLEIGH_MAX_LUM;
	M = (1.0 - exp(-opticalDepth * densityM * C_MIE / MIE_MAX_LUM)) * MIE_MAX_LUM;
}

// Main atmosphere function
float3 GetAtmosphere(
    float3 rayStart,      // Camera position
    float3 rayDir,        // View direction
    float  rayLength,     // View distance
    float3 lightDir,      // Light (sun) direction
	float3 lightColor,    // Light (sun) color. Usually white
out float4 transmittance, // Atmospheric transmittance in xyz, planet intersection flag in w
    float4 fogFactor,     // (Optional) Fog "fade" factor. Can be used to add your own height fog or to fade the world out
    float  occlusion      // (Optional) Scattering occlusion (god rays)
) {
#ifdef PREVENT_CAMERA_GROUND_CLIP
	rayStart.y = max(rayStart.y, 1.0);
#endif

	// Planet and atmosphere intersection to get optical depth
	// TODO: Could simplify to circle intersection test if flat horizon is acceptable
	float2 t1 = PlanetIntersection(rayStart, rayDir);
	float2 t2 = AtmosphereIntersection(rayStart, rayDir);
    
    // Note: This only works if camera XZ is at 0. Otherwise, swap for the line below.
    float altitude = rayStart.y;
    //float altitude = (length(rayStart - PLANET_CENTER) - PLANET_RADIUS);
    float normAltitude = rayStart.y / ATMOSPHERE_HEIGHT;

	if (t2.y < 0.0)
	{
		// Outside of atmosphere looking into space, return nothing
		transmittance = float4(1, 1, 1, 1);
		return float3(0, 0, 0);
	}
    else
    {
        // In case camera is outside of atmosphere, subtract distance to entry.
        t2.y -= max(0.0, t2.x);

#ifdef DRAW_PLANET
        float opticalDepth = t1.x > 0.0 ? min(t1.x, t2.y) : t2.y;
#else
        float opticalDepth = t2.y;
#endif

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
        lightColor *= GetLightTransmittance(rayStart, float3(lightDir.x, ly, lightDir.z), hbias, M_OZONE2);

#ifndef LIGHT_COLOR_IS_RADIANCE
        // If used in an environment where light "color" is not defined in radiometric units
        // we need to multiply with PI to correct the output.
        lightColor *= PI;
#endif

        float3 R, M;
        GetRayleighMie(opticalDepth, densityR, densityM, R, M);
        
        float3 E = (C_RAYLEIGH * densityR + C_MIE * densityM + C_OZONE * densityR * M_OZONE) * pow4(1.0 - normAltitude) * M_TRANSMITTANCE;

        float costh = dot(rayDir, lightDir);
        float phaseR = PhaseR(costh);
        float phaseM = PhaseM(costh, 0.88);
        
#ifdef NIGHT_LIGHT
        float nightLight = NIGHT_LIGHT;
#else
        float nightLight = 0.0;
#endif
        
        // Combined scattering
        float3 rayleigh = (phaseR * occlusion + phaseR * M_FAKE_MS) * lightColor + nightLight * phaseR;
        float3 mie = ((phaseM * occlusion + phaseR * M_FAKE_MS) * lightColor + nightLight * phaseR) * M_MIE;
        float3 scattering = mie * M + rayleigh * R;

        // View extinction, matched to reference
        transmittance.xyz = exp(-(opticalDepth + pow8(opticalDepth * 4.5e-6)) * E);
        // Store planet intersection flag in transmittance.w, useful for occluding clouds, celestial bodies etc.
        transmittance.w = step(t1.x, 0.0);

        if (fogFactor.w > 0.0)
        {
            // 2nd sample (all the way to atmosphere exit), used for fog fade.
            opticalDepth = t2.y;
            GetRayleighMie(opticalDepth, densityR, densityM, R, M);
            float3 scattering2 = mie * M + rayleigh * R;
            float3 transmittance2 = exp(-opticalDepth * E);

            scattering2 *= lerp(fogFactor.xyz, float3(1, 1, 1), sq(fogFactor.w)); // Fog color test
            scattering = lerp(scattering, scattering2, fogFactor.w);
            transmittance.xyz = lerp(transmittance.xyz, transmittance2, fogFactor.w);
        }
        
        if (t1.y > 0.0 && t1.y < rayLength)
        {
            // Darken planet
            float3 planetColor = float3(0.4, 0.4, 0.4);
            float planetOpticalDepth = t1.y - max(0.0, t1.x);
            float skyWeight = exp(-planetOpticalDepth * 1e-6);
            scattering *= lerp(planetColor, float3(1, 1, 1), skyWeight);
        }

        return scattering * M_EXPOSURE_MUL;
    }
}

// Overloaded functions
float3 GetAtmosphere(
    float3 rayStart,
    float3 rayDir,
    float rayLength,
    float3 lightDir,
	float3 lightColor,
out float4 transmittance
) {
    return GetAtmosphere(rayStart, rayDir, rayLength, lightDir, lightColor, transmittance, vec4(0.0), 1.0);
}
float3 GetAtmosphere(
    float3 rayStart,
    float3 rayDir,
    float rayLength,
    float3 lightDir,
    float3 lightColor
) {
    float4 transmittance;
    return GetAtmosphere(rayStart, rayDir, rayLength, lightDir, lightColor, transmittance, vec4(0.0), 1.0);
}

float3 GetSunDisc(float3 rayDir, float3 lightDir)
{
    const float A = cos(0.00436 * SUN_DISC_SIZE);
	float costh = dot(rayDir, lightDir);
	float disc = sqrt(smoothstep(A, 1.0, costh));
	return float3(disc, disc, disc);
}

struct Light
{
    vec3 direction;
    vec3 radiance;
};

#endif // FAST_ATMOSPHERE_INCLUDED