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

		Texture(Texture&& other) noexcept;

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		void Bind(int unit = 0) const;
		void Unbind() const;
		glm::ivec2 Size() const;
		GLuint GetID() const;

	private:
		GLuint id;
		glm::ivec2 size;
	};
}