#pragma once

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Eon
{
	class FileUtil
	{
	public:
		static std::string GetFileContents(const std::string& filePath);
	};
}