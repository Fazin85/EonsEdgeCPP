#version 330 core

uniform sampler2D textureSampler;

in vec3 pos;
in vec2 texCoord;
in vec3 color;
in vec3 normal;

void main()
{
	vec4 tex = texture(textureSampler, texCoord);
	tex *= color;
	
	gl_FragColor = tex;
}