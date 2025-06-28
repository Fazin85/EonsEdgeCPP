#pragma once

#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <string>

namespace Eon
{
	class Texture
	{
	public:
		Texture(const sf::Image& image, bool mipmaps);
		~Texture();

		void Bind() const;
		void Unbind() const;
		glm::ivec2 Size() const;

	private:
		GLuint id;
		glm::ivec2 size;
	};
}