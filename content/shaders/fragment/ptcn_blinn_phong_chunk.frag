#version 430 core

#include "../common/blinn_phong.glsl"

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gPosition; // w is depth

layout(std430, binding = 0) readonly buffer BlockIDBuffer {
	vec4 blockProperties[];
};

uniform sampler2D textureSampler;
uniform sampler2D blockIDSampler;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 objectColor;

in vec3 pos;
in vec2 texCoord;
in vec3 color;
in vec3 normal;

float getBlockShininess(int blockID) {
	return blockProperties[blockID].x;
}

void main()
{
	vec4 tex = texture(textureSampler, texCoord);
	int blockID = int(texture(blockIDSampler, texCoord).r * 255.0);
	
	float shininess = getBlockShininess(blockID);

	vec3 blinnPhong = blinnPhongDirectional(lightDirection, lightColor, pos, normal, objectColor, shininess);
	tex.rgb *= color;
	tex.rgb *= blinnPhong;
	
	gAlbedo = tex;
	gNormal = vec4(normal, 1.0);
	gPosition = vec4(pos, gl_FragCoord.z);
}