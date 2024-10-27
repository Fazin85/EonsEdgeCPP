#pragma once

#include <vector>

namespace Eon
{
	class Compress
	{
	public:
		static std::vector<char> Fast(char* data, size_t srcSize);
	};
}