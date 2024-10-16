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
		PlaneMesh(Image& image);
		~PlaneMesh();
		void Render(Camera& camera, glm::vec3 cameraPosition);

	private:
		unsigned int vbo;
		unsigned int vao;
		unsigned int ebo;
		std::unique_ptr<Texture> texture;

		static inline Shader* shader = nullptr;

		static inline const float vertices[]{
			// positions          // texture coords
			 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
			 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
			-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
		};

		static inline const unsigned int indices[]{
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};
	};
}