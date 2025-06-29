#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec3 aNormal;

out vec3 pos;
out vec2 texCoord;
out vec3 color;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{	
	pos = aPos;
	texCoord = aTexCoord;
	color = aColor;
	normal = aNormal;
	
    vec4 p = projection * view * model * vec4(aPos, 1.0f);
	
	gl_Position = p;
}
