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

u32 Image::Width() const
{
	return width;
}

u32 Image::Height() const
{
	return height;
}

u8* Image::Data()
{
	return data;
}
