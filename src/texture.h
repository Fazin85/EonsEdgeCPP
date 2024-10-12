#pragma once

#include <string>
#include <glad/glad.h>
#include "image.h"

class Texture
{
public:
	Texture(Image& image);
	~Texture();

	void Bind() const;
	void Unbind();

private:
	GLuint id;
};