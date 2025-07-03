#version 330 core

layout(location = 0) out vec4 gAlbedo;

uniform vec3 objectColor;

void main()
{
	gAlbedo = vec4(objectColor, 1.0);
}