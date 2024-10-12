#pragma once

#include <string>
#include "num.h"

class Image
{
public:
	Image(const std::string& filePath);
	~Image();
	u32 Width() const;
	u32 Height() const;
	u8* Data();
private:
	int width;
	int height;
	unsigned char* data;
};

