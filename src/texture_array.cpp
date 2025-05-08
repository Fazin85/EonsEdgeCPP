#include "texture_array.h"
#include <glad/glad.h>
#include "stb_image.h"
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
			int x;
			int y;
			int channels;
			std::string filePath = "content/images/" + imagefilePaths[i];
			unsigned char* imageData = stbi_load(filePath.c_str(), &x, &y, &channels, STBI_rgb_alpha);

			if (imageData == nullptr || channels != STBI_rgb_alpha || x != width || y != height)
			{
				EON_ERROR("Failed to load image: " + imagefilePaths[i]);
				return;
			}

			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, static_cast<GLsizei>(i), width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

			stbi_image_free(imageData);
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
