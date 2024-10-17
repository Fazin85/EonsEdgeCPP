#include "log.h"
#include "plane_mesh.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Eon
{
	PlaneMesh::PlaneMesh(glm::vec3 position, glm::vec2 scale, Image& image) : model_matrix(1.0f)
	{
		if (shader == nullptr)
		{
			shader = new Shader("Plane.vert", "Plane.frag");
		}

		float vertices[]{
			 position.x + scale.x,  position.y + scale.y, position.z,   1.0f * scale.x, 1.0f * scale.y,
			 position.x + scale.x, position.y - scale.y, position.z,   1.0f * scale.x, 0.0f,
			 position.x - scale.x, position.y - scale.y, position.z,   0.0f, 0.0f,
			 position.x - scale.x,  position.y + scale.y, position.z,   0.0f, 1.0f * scale.y
		};

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		texture = std::make_unique<Texture>(image, true);

		shader->Bind();
		shader->UniformInt1("texture0", 0);
		shader->UniformMatrix4NoTranspose("model", model_matrix);
	}

	PlaneMesh::~PlaneMesh()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}

	void PlaneMesh::Render(Camera& camera, glm::vec3 cameraPosition)
	{
		glActiveTexture(GL_TEXTURE0);
		texture->Bind();

		shader->Bind();

		camera.CalculateViewMatrix(cameraPosition);

		shader->UniformMatrix4NoTranspose("model", model_matrix);
		shader->UniformMatrix4NoTranspose("view", *camera.ViewMatrix());
		shader->UniformMatrix4NoTranspose("projection", *camera.ProjectionMatrix());

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void PlaneMesh::Render(Camera& camera, glm::vec3 cameraPosition, Shader& shader)
	{
		glActiveTexture(GL_TEXTURE0);
		texture->Bind();

		shader.Bind();

		camera.CalculateViewMatrix(cameraPosition);

		shader.UniformMatrix4NoTranspose("model", model_matrix);
		shader.UniformMatrix4NoTranspose("view", *camera.ViewMatrix());
		shader.UniformMatrix4NoTranspose("projection", *camera.ProjectionMatrix());

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void PlaneMesh::Rotate(int axis, float degrees)
	{
		switch (axis)
		{
		case 0://x
			model_matrix = glm::rotate(model_matrix, glm::radians(degrees), glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		case 1://y
			model_matrix = glm::rotate(model_matrix, glm::radians(degrees), glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case 2://z
			model_matrix = glm::rotate(model_matrix, glm::radians(degrees), glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		default:
			EON_ERROR("Invalid axis: " + std::to_string(axis));
			break;
		}
	}

	void PlaneMesh::Scale(glm::vec3 amount)
	{
		model_matrix = glm::scale(model_matrix, amount);
	}
}
