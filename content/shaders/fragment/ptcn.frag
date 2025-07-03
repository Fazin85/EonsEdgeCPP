#version 330 core

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gPosition; // w is depth

uniform sampler2D albedoMap;

in vec3 pos;
in vec2 texCoord;
in vec3 color;
in vec3 normal;

void main()
{
	vec4 tex = texture(albedoMap, texCoord);
	tex.rgb *= color;
	
	gAlbedo = tex;
	gNormal = vec4(normal, 1.0);
	gPosition = vec4(pos, gl_FragCoord.z);
}