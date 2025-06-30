#version 330 core

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gPosition; // w is depth

uniform sampler2D textureSampler;

in vec3 pos;
in vec2 texCoord;
in vec3 color;
in vec3 normal;

void main()
{
	vec4 tex = texture(textureSampler, texCoord);
	tex.rgb *= color;
	
	gAlbedo = tex;
	gNormal = normal;
	gPosition = vec4(pos, gl_FragCoord.z);
}