#pragma once

#include "camera.h"
#include "shader.h"
#include "texture.h"
#include <memory>

namespace Eon
{
	class PlaneMesh
	{
	public:
		PlaneMesh(glm::vec3 position, glm::vec2 scale, Image& image);
		~PlaneMesh();
		void Render(Camera& camera, glm::vec3 cameraPosition);
		void Render(Camera& camera, glm::vec3 cameraPosition, Shader& shader);
		void Rotate(int axis, float degrees);
		void Scale(glm::vec3 amount);

	private:
		unsigned int vbo;
		unsigned int vao;
		unsigned int ebo;
		std::unique_ptr<Texture> texture;
		glm::mat4 model_matrix;

		static inline Shader* shader = nullptr;

		static inline const unsigned int indices[]{
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};
	};
}