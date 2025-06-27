#include "texture.h"
#include "gl_error_check.h"

namespace Eon
{
	Texture::Texture(Image& image, bool mipmaps) : size(image.Width(), image.Height())
	{
		GL_CHECK(glGenTextures(1, &id));

		GL_CHECK(glActiveTexture(GL_TEXTURE0));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, id));

		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));

		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST));

		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

		GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width(), image.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Data()));

		if (mipmaps)
		{
			GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
		}
	}

	Texture::~Texture()
	{
		GL_CHECK(glDeleteTextures(1, &id));
	}

	void Texture::Bind() const
	{
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, id));
	}

	void Texture::Unbind()
	{
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	}

	glm::ivec2 Texture::Size() const
	{
		return size;
	}
}
