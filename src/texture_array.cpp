#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include "texture_array.h"
#include "log.h"

namespace Eon
{
	TextureArray::TextureArray(std::vector<std::string>& imagefilePaths, unsigned int width, unsigned int height)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST/*_MIPMAP_NEAREST*/);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, static_cast<GLsizei>(imagefilePaths.size()), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		for (size_t i = 0; i < imagefilePaths.size(); i++)
		{
			std::string filePath = "content/images/" + imagefilePaths[i];
			sf::Image image;
			image.loadFromFile(filePath);
			if (!image.getPixelsPtr())
			{
				EON_ERROR("Failed to load image: " + imagefilePaths[i]);
				return;
			}

			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, static_cast<GLsizei>(i), image.getSize().x, image.getSize().y, 1, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
		}

		//glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	TextureArray::~TextureArray()
	{
		glDeleteTextures(1, &id);
	}

	void TextureArray::Bind() const
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	}

	void TextureArray::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
}
