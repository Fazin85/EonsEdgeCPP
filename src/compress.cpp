#include "compress.h"
#include "log.h"
#include <lz4.h>

std::vector<char> Eon::Compress::Fast(char* data, size_t srcSize)
{
	size_t dstSize = LZ4_COMPRESSBOUND(srcSize);
	std::vector<char> output;
	output.reserve(dstSize);

	dstSize = LZ4_compress_default(
		data, output.data(), srcSize, dstSize);

	if (dstSize <= 0)
	{
		EON_ERROR("Failed to compress");
		return std::vector<char>();
	}

	output.resize(dstSize);

	return output;
}
