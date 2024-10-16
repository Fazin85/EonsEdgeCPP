#include "plane_mesh.h"

namespace Eon
{
	PlaneMesh::PlaneMesh(Image& image)
	{
		if (shader == nullptr)
		{
			shader = new Shader("Plane.vert", "Plane.frag");
		}

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

		texture = std::make_unique<Texture>(image);

		shader->Bind();
		shader->UniformInt1("texture0", 0);
		shader->UniformMatrix4NoTranspose("model", glm::mat4(1.0f));
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

		shader->UniformMatrix4NoTranspose("view", *camera.ViewMatrix());
		shader->UniformMatrix4NoTranspose("projection", *camera.ProjectionMatrix());

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}
