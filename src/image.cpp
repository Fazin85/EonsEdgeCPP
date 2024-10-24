#define STB_IMAGE_IMPLEMENTATION

#include "image.h"
#include "log.h"
#include "stb_image.h"

Image::Image(const std::string& filePath)
{
	std::string realFilePath = "content/images/" + filePath;
	stbi_set_flip_vertically_on_load(true);

	int channels;
	data = stbi_load(realFilePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

	if (data == nullptr)
	{
		EON_ERROR("Failed to load image: " + filePath);
		return;
	}

	EON_INFO("Loaded image: " + filePath);
}

Image::~Image()
{
	if (data != nullptr)
	{
		stbi_image_free(data);
		data = nullptr;
	}
}

unsigned int Image::Width() const
{
	return width;
}

unsigned int Image::Height() const
{
	return height;
}

unsigned char* Image::Data()
{
	return data;
}
