#pragma once

#include <string>


class Image
{
public:
	Image(const std::string& filePath);
	~Image();
	unsigned int Width() const;
	unsigned int Height() const;
	unsigned char* Data();
private:
	int width;
	int height;
	unsigned char* data;
};

