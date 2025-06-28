#pragma once

#include <SFML/Graphics.hpp>
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include <memory>

namespace Eon
{
	class PlaneMesh
	{
	public:
		PlaneMesh(glm::vec3 position, glm::vec2 scale, sf::Image& image);
		~PlaneMesh();
		void Render(Camera& camera, glm::vec3 cameraPosition);
		void Rotate(int axis, float degrees);

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