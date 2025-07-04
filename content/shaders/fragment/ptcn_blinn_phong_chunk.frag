#version 430 core

#include "../common/blinn_phong.glsl"

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gPosition; // w is depth

layout(std430, binding = 0) readonly buffer BlockIDBuffer {
	vec4 blockProperties[];
};

uniform sampler2D albedoMap;

// r is block id / 255.0
// g is block shininess / 255.0
uniform sampler2D blockSampler;

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float elapsedTime;

in vec3 pos;
in vec2 texCoord;
in vec3 color;
in vec3 normal;

void main()
{
	vec4 tex = texture(albedoMap, texCoord);
	vec4 blockData = texture(blockSampler, texCoord);
	
	int blockID = int(blockData.r * 255.0);
	float shininess = blockData.g * 255.0;

	vec3 blinnPhong = blinnPhongDirectional(lightDirection, lightColor, pos, normal, objectColor, shininess);
	tex.rgb *= color;
	tex.rgb *= blinnPhong;
	
	gAlbedo = tex;
	gNormal = vec4(normal, 1.0);
	gPosition = vec4(pos, gl_FragCoord.z);
}