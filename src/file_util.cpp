#include "file_util.h"
#include "log.h"

std::string Eon::FileUtil::GetFileContents(const std::string& filePath)
{
	std::ifstream inFile(filePath);
	if (!inFile.is_open())
	{
		EON_ERROR("Unable to open file: " + filePath);
		return "";
	}

	std::stringstream stream;
	stream << inFile.rdbuf();
	return stream.str();
}
