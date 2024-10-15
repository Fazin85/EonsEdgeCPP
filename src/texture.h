#pragma once

#include "image.h"
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <string>

namespace Eon
{
	class Texture
	{
	public:
		Texture(Image& image);
		~Texture();

		void Bind() const;
		void Unbind();
		glm::ivec2 Size() const;

	private:
		GLuint id;
		glm::ivec2 size;
	};
}