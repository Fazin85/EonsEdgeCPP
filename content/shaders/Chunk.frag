#version 330 core

in vec2 texCoord;
in vec4 v_color;
in vec3 v_viewpos;
in vec3 v_normal;
in vec3 frag_pos;
in vec3 worldPos;
flat in uint blockType;

out vec4 FragColor;

uniform vec3 sunlight_dir;
uniform vec4 fog_color;
uniform float ambient_light;
uniform float fog_near;
uniform float fog_far;
uniform vec3 light_color;
uniform sampler2DArray textureSampler;

const float MIN_DIFFUSE_LIGHT = 0.35;
const uint WATER_TYPE = 5U;

void main() 
{
	FragColor = texture(textureSampler, vec3(texCoord, blockType)) * v_color;
	
	if(FragColor.a == 0.0) {
		discard;
	}
	
	//diffuse lighting
	vec3 norm = normalize(v_normal);
	vec3 lightDir = normalize(-sunlight_dir);
	
	float diff = max(dot(norm, lightDir), MIN_DIFFUSE_LIGHT);
	vec3 d = diff * light_color;
	vec4 diffuse = vec4(d, 1.0);
	
	//ambient lighting
	vec4 ambient = FragColor * ambient_light;	
	
	if(blockType == WATER_TYPE) {
		vec3 viewDir = normalize(v_viewpos - worldPos);
		vec3 halfVector = normalize(lightDir + viewDir);
		float specular = pow(max(0.0, dot(norm, halfVector)), 100.0);
		vec3 specularColor = vec3(1.0) * specular * 0.7;
		
		FragColor.rgb += specularColor;
	}

	//fog
	float fog = smoothstep(fog_near, fog_far, length(v_viewpos));
	//FragColor = (diffuse + ambient) * FragColor;
	FragColor = mix(FragColor, fog_color, fog);
}