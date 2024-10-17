#include "texture.h"

namespace Eon
{
	Texture::Texture(Image& image, bool mipmaps) : size(image.Width(), image.Height())
	{
		glGenTextures(1, &id);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width(), image.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Data());

		if (mipmaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &id);
	}

	void Texture::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void Texture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glm::ivec2 Texture::Size() const
	{
		return size;
	}
}
