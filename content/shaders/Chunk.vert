#version 330

layout (location = 0) in uint vertexPositionData;
layout (location = 1) in uint uvDirLightData;

out vec2 texCoord;
out vec4 v_color;
out vec3 v_viewpos;
out vec3 v_normal;
out vec3 frag_pos;
flat out uint blockType;

// uniform variables
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 chunkPos;
uniform vec3 camPos;

const uint FRONT = 0U;
const uint BACK = 1U;
const uint LEFT = 2U;
const uint RIGHT = 3U;
const uint TOP = 4U;
const uint BOTTOM = 5U;

void main() 
{
	uint x = (vertexPositionData & (0xFFU << 24U)) >> 24U;
	uint z = (vertexPositionData & 0xFF0000U) >> 16U;
	uint y = ((vertexPositionData & 0xFF00U) >> 8U) | (vertexPositionData & 0xFFU) << 8U;
	
	vec3 vertexPosition = vec3(x, y, z);
	vec4 worldPosition = vec4(vertexPosition + chunkPos - camPos, 1.0);
	gl_Position = worldPosition * model * view * projection; // coordinates
	
	uint inLight = (uvDirLightData & (0xFFU << 24U)) >> 24U;

	const float min_light = 0.1;
	vec3 light = vec3(inLight / 15.0, inLight / 15.0, inLight / 15.0);
    light *= vec3(1.0 - min_light);
    light += vec3(min_light);
	
	uint direction = (uvDirLightData & 0xFF0000U) >> 16U;
	
	if(direction == TOP) {
		light *= 1.0;
	}
	else if(direction == LEFT || direction == RIGHT) {
		light *= 0.95;
	}
	else if(direction == FRONT || direction == BACK) {
		light *= 0.85;
	}
	else {
		light *= 0.75;
	}

	uint valX = ((uvDirLightData & 0xFF00U) >> 8U);
	blockType = (uvDirLightData & 0xFFU);
	
	if(valX == 0U) {
		texCoord = vec2(0.0, 1.0);
	}
	else if(valX == 1U) {
		texCoord = vec2(0.0, 0.0);
	}
	else if(valX == 2U) {
		texCoord = vec2(1.0, 0.0);
	}
	else if(valX == 3U) {
		texCoord = vec2(1.0, 1.0);
	}

	v_color = vec4(light, 1.0);
	v_viewpos = ((view * model) * worldPosition).xyz;
	
	frag_pos = vec3(vec4(worldPosition) * model);
	
	vec3 normal = vec3(0.0, 0.0, -1.0);
	
	if(direction == BACK) {
		normal = vec3(0.0, 0.0, 1.0);
	}
	else if(direction == LEFT) {
		normal = vec3(-1.0, 0.0, 0.0);
	}
	else if(direction == RIGHT) {
		normal = vec3(1.0, 0.0, 0.0);
	}
	else if(direction == BOTTOM) {
		normal = vec3(0.0, -1.0, 0.0);
	}
	else if(direction == TOP) {
		normal = vec3(0.0, 1.0, 0.0);
	}
	
	v_normal = normal * mat3(transpose(inverse(model)));
}