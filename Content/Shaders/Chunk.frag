#version 330 core

in vec2 texCoord;
in vec4 v_color;
in vec3 v_viewpos;
in vec3 v_normal;
in vec3 frag_pos;

out vec4 FragColor;

uniform vec3 sunlight_dir;
uniform vec4 fog_color;
uniform float ambient_light;
uniform float fog_near;
uniform float fog_far;
uniform vec3 light_color;
uniform sampler2D texture0;

const float MIN_DIFFUSE_LIGHT = 0.35;

void main() 
{
	FragColor = texture(texture0, texCoord) * v_color;
	
	//diffuse lighting
	vec3 norm = normalize(v_normal);
	vec3 lightDir = normalize(-sunlight_dir);
	
	float diff = max(dot(norm, lightDir), MIN_DIFFUSE_LIGHT);
	vec3 d = diff * light_color;
	vec4 diffuse = vec4(d, 1.0);
	
	//ambient lighting
	vec4 ambient = FragColor * ambient_light;
	
	//fog
	float fog = smoothstep(fog_near, fog_far, length(v_viewpos));
	FragColor = (diffuse + ambient) * FragColor;
    FragColor = mix(FragColor, fog_color, fog);
}