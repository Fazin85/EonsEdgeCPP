#pragma once

#include "shader.h"
#include "asset_manager.h"
#include <string>

namespace Eon
{
	class TextRenderer
	{
	public:
		explicit TextRenderer(const std::string& fontFileName);
		~TextRenderer();

		void Begin() const;
		void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
		void End() const;

	private:
		struct Character
		{
			unsigned int texture_id;
			glm::ivec2 size;
			glm::ivec2 bearing;
			unsigned int advance;
		};
		
		unsigned int vao;
		unsigned int vbo;
		ShaderID shader;
		std::unordered_map<char, Character> characters;
	};
}