#pragma once

#include "camera.h"
#include "image.h"
#include "shader.h"
#include "texture.h"
#include <memory>

namespace Eon
{
	class BillboardSprite
	{
	public:
		BillboardSprite(Image& image);
		~BillboardSprite();
		glm::vec3& Position();
		glm::vec2& Size();
		void Render(Camera& camera, glm::vec3 cameraPosition);

	private:
		std::unique_ptr<Texture> texture;
		glm::vec3 position;
		glm::vec2 size;
		GLuint vertex_buffer;
		GLuint vertex_array;

		static inline Shader* shader = nullptr;
		static inline const float vertex_buffer_data[20]{ -0.5f, -0.5f, 0.0f,0.5f, -0.5f, 0.0f,-0.5f,  0.5f, 0.0f,0.5f,  0.5f, 0.0f, };
	};
}