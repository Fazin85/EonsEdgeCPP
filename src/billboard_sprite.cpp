#include "billboard_sprite.h"

Eon::BillboardSprite::BillboardSprite(Image& image) : position(0, 0, 0), size(1.0f, 1.0f)
{
	texture = std::make_unique<Texture>(image, true);

	if (shader == nullptr)
	{
		shader = new Shader("Billboard.vert", "Billboard.frag");
	}

	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_DYNAMIC_DRAW);
}

Eon::BillboardSprite::~BillboardSprite()
{
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteVertexArrays(1, &vertex_array);
}

glm::vec3& Eon::BillboardSprite::Position()
{
	return position;
}

glm::vec2& Eon::BillboardSprite::Size()
{
	return size;
}

void Eon::BillboardSprite::Render(Camera& camera, glm::vec3 cameraPosition)
{
	camera.CalculateViewMatrix(cameraPosition);
	const glm::mat4& viewMatrix = camera.ViewMatrix();
	const glm::mat4& viewProjectionMatrix = camera.ProjectionMatrix() * viewMatrix;

	glActiveTexture(GL_TEXTURE0);
	texture->Bind();

	shader->Bind();

	shader->UniformInt1("textureSampler", 0);
	shader->UniformFVec3("CameraRight_worldspace", glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]));
	shader->UniformFVec3("CameraUp_worldspace", glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]));

	shader->UniformFVec3("BillboardPos", position);
	shader->UniformFVec2("BillboardSize", size);

	shader->UniformMatrix4NoTranspose("VP", viewProjectionMatrix);

	glBindVertexArray(vertex_array);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(0);
}

