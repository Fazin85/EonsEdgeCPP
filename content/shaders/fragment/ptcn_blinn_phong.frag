#version 330 core

#include "../common/blinn_phong.glsl"

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gPosition; // w is depth

uniform sampler2D textureSampler;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform float shininess;

in vec3 pos;
in vec2 texCoord;
in vec3 color;
in vec3 normal;

void main()
{
	vec4 tex = texture(textureSampler, texCoord);
	vec3 blinnPhong = blinnPhongDirectional(lightDirection, lightColor, viewPos, pos, normal, objectColor, shininess);
	tex.rgb *= color;
	tex.rgb *= blinnPhong;
	
	gAlbedo = tex;
	gNormal = vec4(normal, 1.0);
	gPosition = vec4(pos, gl_FragCoord.z);
}