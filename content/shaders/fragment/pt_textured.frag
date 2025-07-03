#version 330 core

layout(location = 0) out vec4 gAlbedo;

in vec2 texCoord;

uniform sampler2D albedoMap;

void main()
{
	gAlbedo = texture(albedoMap, texCoord);
}