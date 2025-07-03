#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 pos;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{	
	pos = (model * vec4(aPos, 1.0f)).xyz;
	texCoord = aTexCoord;

    vec4 p = projection * view * model * vec4(aPos, 1.0f);
	
	gl_Position = p;
}
