//uniform vec3 RWeatherParams;

const float waterNormalOctaves = 5;
const float pi      = radians(180.0);
const float tau     = radians(360.0);

float rcp(int x)    { return 1.0 / (float(x)); }
float rcp(float x)  { return 1.0 / x; }
vec2 rcp(vec2 x)    { return 1.0 / x; }
vec3 rcp(vec3 x)    { return 1.0 / x; }

vec2 cubeSmooth(vec2 x) 
{
    return (x * x) * (3.0 - 2.0 * x);
}

float cubeSmooth(float x) 
{
    return (x * x) * (3.0 - 2.0 * x);
}

vec2 waveNoiseCubic(sampler2D tex, vec2 pos) {
    pos        *= 256.0;
    ivec2 location  = ivec2(floor(pos));

    vec2 samples[4]    = vec2[4](
        texelFetch(tex, location                 & 255, 0).xy, texelFetch(tex, (location + ivec2(1, 0)) & 255, 0).xy,
        texelFetch(tex, (location + ivec2(0, 1)) & 255, 0).xy, texelFetch(tex, (location + ivec2(1, 1)) & 255, 0).xy
    );

    vec2 weights    = cubeSmooth(fract(pos));


    return mix(
        mix(samples[0], samples[1], weights.x),
        mix(samples[2], samples[3], weights.x), weights.y
    );
}

float waveGerstener(vec2 pos, float t, float steepness, float amp, float length, vec2 dir) {
    float k     = tau * rcp(length);
    float w     = sqrt(9.81 * k);
    
    float x     = w * t - k * dot(dir, pos);
        x       = pow((sin(x) * 0.5 + 0.5), steepness);
    float cubicLerp     = 1.0 - saturate(abs(steepness - 1.0));
    return mix(x, cubeSmooth(x), saturate(cubicLerp * sqrt(cubicLerp))) * amp;
}

float waterWaves(vec3 pos, float viewDist) {
    float time = elapsedTime * 0.76;

    vec2 p      = pos.xz+pos.y*rcp(pi);

    vec2 dir    = normalize(vec2(0.4, 0.8));

    vec2 noise = (waveNoiseCubic(noisetex, (p + dir * time * 0.2) * 0.0008).rg * 2.0 - 1.0);
        p     += noise * 1.3;

    float wave  = 0.0;

    float amp   = 0.06;
    float steep = 0.51;
    float wlength = 2.8;

    const float a = 2.6;
    const mat2 rotation = mat2(cos(a), -sin(a), sin(a), cos(a));
    float noiseAmp = 4.5;

    float total     = 0.0;

    vec2 noiseCoord = (p + dir * time * 0.4) * 0.0007;

    float distFalloff   = 1.0 - exp(-viewDist * rcp(32.0));

    float ampMult   = mix(0.6, 0.9, distFalloff);

    float mult  = 1.0 - distFalloff * 0.9;

    for (uint i = 0; i<waterNormalOctaves; ++i) {
        vec2 noise = waveNoiseCubic(noisetex, noiseCoord).rg * 2.0 - 1.0;

        steep   = mix(steep, sqrt(steep), sqrt(saturate(abs(wave))));
        
        float temp = waveGerstener(p + noise * noiseAmp + vec2(wave * 0.9, 0.0), time, steep, amp, wlength, dir);
        p += temp * dir * amp * pi;
        wave   -= temp;
        if (i < 2) wave -= waveNoiseCubic(noisetex, (p + noise * noiseAmp * 0.5 + wave) * 0.023 - time * dir * 0.03).r * amp * 0.3;

        time   *= 1.1;

        //amp    *= 0.5 + RWeatherParams.z * 0.1;
        amp    *= 0.55;

        wlength *= 0.63;

        dir    *= rotation;
        noiseCoord *= rotation;
        noiseCoord *= 1.5;
        noiseAmp *= 0.65;
    }

    return (wave - amp) * mult;
}

const float waterParallaxDepth = 2.0;

vec3 waterParallax(vec3 pos, vec3 dir, float viewDist) {    //based on spectrum by zombye
    const uint steps    = 8;

    vec3 interval   = inversesqrt(float(steps)) * dir / -dir.y;
    float height    = waterWaves(pos, viewDist) * waterParallaxDepth;
    float stepSize  = height;
        pos.xz     += stepSize * interval.xz;

    float offset    = stepSize * interval.y;
        height      = waterWaves(pos, viewDist) * waterParallaxDepth;

    for (uint i = 1; i < steps - 1 && height < offset; ++i) {
        stepSize    = offset - height;
        pos.xz     += stepSize * interval.xz;

        offset     += stepSize * interval.y;
        height      = waterWaves(pos, viewDist) * waterParallaxDepth;
    }

    if (height < offset) {
        stepSize    = offset - height;
        pos.xz     += stepSize * interval.xz;
    }

    return pos;
}

vec3 waterNormal(vec3 worldPos, vec3 tangentViewDir, float viewDist) {
    vec3 pos        = waterParallax(worldPos, tangentViewDir.xzy, viewDist);

    float dstep   = 0.015 + clamp(viewDist * rcp(32.0), 0.0, 2.0) * 0.045;

    vec2 delta;
        delta.x     = waterWaves(pos + vec3( dstep, 0.0, -dstep), viewDist);
        delta.y     = waterWaves(pos + vec3(-dstep, 0.0,  dstep), viewDist);
        delta      -= waterWaves(pos + vec3(-dstep, 0.0, -dstep), viewDist);

    return normalize(vec3(-delta.x, 2.0 * dstep, -delta.y));
}

const float waterCoeffRed = 8; //[1 2 3 4 5 6 7 8 9 10]
const float waterCoeffGreen = 4; //[1 2 3 4 5 6 7 8 9 10]
const float waterCoeffBlue = 3; //[1 2 3 4 5 6 7 8 9 10]
const float waterCoeffScatter = 3; //[1 2 3 4 5 6 7 8 9 10]

const vec3 waterExtinctionCoeff = vec3(waterCoeffRed, waterCoeffGreen, waterCoeffBlue) * 1e-2;
const vec3 waterScatterCoeff = vec3(waterCoeffScatter) * 1e-2;
const vec3 waterAttenCoeff = (waterExtinctionCoeff + waterScatterCoeff) * 4.0;
const vec3 waterAbsorbCoeff = (waterExtinctionCoeff + waterScatterCoeff);

// Utility functions

const uint waterVolMinSteps = 3;
const float waterVolAdaptiveSteps = 9;
const float waterVolClipDist = 48.0;

float maxOf(vec2 a)         { return max(a.x, a.y); }
float maxOf(vec3 a)         { return max(a.x, max(a.y, a.z)); }
float maxOf(float a, float b, float c) { return max(a, max(b, c)); }
float maxOf(vec4 a)         { return max(a.x, max(a.y, max(a.z, a.w))); }

float sqr(float x)          { return x * x; }

const float rLog2 = 1.0/(log(2.0));

vec3 expf(vec3 x) 
{
    return exp2((x)*rLog2);
}

vec2 expf(vec2 x) 
{
    return exp2((x)*rLog2);
}

float mieHG(float cosTheta, float g) {
    float mie   = 1.0 + sqr(g) - 2.0*g*cosTheta;
        mie     = (1.0 - sqr(g)) / ((4.0*pi) * mie*(mie*0.5+0.5));
    return mie;
}

vec3 waterColor(vec3 scenePos, vec3 startPos0, vec3 worldDir, bool isSky, float dither, vec3 skyColor, float vDotL) 
{
    float endDist   = isSky ? waterVolClipDist : length(scenePos);

    vec3 startPos   = startPos0;
    vec3 endPos     = isSky ? startPos0 + worldDir * waterVolClipDist : scenePos;
        if (distance(startPos, endPos) > waterVolClipDist) endPos = startPos0 + waterVolClipDist * worldDir;

    float baseStep  = length(endPos - startPos);
    float stepCoeff = saturate(baseStep / waterVolClipDist);

    uint steps      = waterVolMinSteps + uint(stepCoeff * waterVolAdaptiveSteps);

    vec3 rStep      = (endPos - startPos) / float(steps);
    vec3 rPos       = startPos + rStep * dither;
    float rLength   = length(rStep);

    vec3 transmittance = vec3(1.0);

    float phase     = mieHG(vDotL, 0.7);
    float phaseIso  = 0.25 * rcp(pi);
        phase       = mix(phase, phaseIso, 0.3);
    mat2x3 scattering = mat2x3(0.0);
    uint i = 0;
    do {
        rPos += rStep; //shadowPos += shadowStep;
        if (length(rPos) > length(scenePos) && !isSky) break;
        if (distance(startPos, rPos) > waterVolClipDist) continue;
        if (maxOf(transmittance) < 0.01) break;

        float stepRho = rLength;

        scattering[1]  += transmittance;

        transmittance  *= expf(-waterAttenCoeff * stepRho);

    } while (++i < steps);

    vec3 attenMul   = (1.0 - expf(-waterAttenCoeff * rLength)) * rcp(waterAttenCoeff);

    scattering[0]  *= waterScatterCoeff * attenMul * phase;
    scattering[1]  *= waterScatterCoeff * attenMul;

    vec3 color  = scattering[1] * skyColor;
    return color;
}