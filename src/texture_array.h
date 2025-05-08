#pragma once

#include <vector>
#include <string>

namespace Eon
{
	class TextureArray
	{
	public:
		TextureArray(std::vector<std::string>& imagefilePaths, unsigned int width, unsigned int height);
		~TextureArray();
		void Bind() const;
		void Unbind() const;

	private:
		unsigned int id;
	};
}