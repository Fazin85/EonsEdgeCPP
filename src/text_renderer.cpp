#include "text_renderer.h"
#include "log.h"
#include "window.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Eon
{
	TextRenderer::TextRenderer(const std::string& fontFileName) : shader("Text.vert", "Text.frag")
	{
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			EON_ERROR("Could not init FreeType Library");

		FT_Face face;
		if (FT_New_Face(ft, ("content/fonts/" + fontFileName).c_str(), 0, &face))
			EON_ERROR("Failed to load font");

		FT_Set_Pixel_Sizes(face, 0, 48);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				EON_ERROR("Failed to load Glyph");
				continue;
			}

			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			characters.try_emplace(
				c,
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
			);
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	TextRenderer::~TextRenderer()
	{
		for (const auto& [c, character] : characters)
		{
			glDeleteTextures(1, &character.texture_id);
		}
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	void TextRenderer::Begin()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		shader.Bind();
		const sf::Vector2u windowSize = Window::Get().getSize();
		shader.UniformMatrix4("projection", glm::ortho(0.0f, static_cast<float>(windowSize.x), 0.0f, static_cast<float>(windowSize.y)));
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);
	}

	void TextRenderer::RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color)
	{
		shader.UniformFVec3("textColor", { color.x, color.y, color.z });
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(vao);

		for (char c : text)
		{
			Character ch = characters[c];

			float xpos = x + static_cast<float>(ch.bearing.x) * scale;
			float ypos = y - static_cast<float>((ch.size.y - ch.bearing.y)) * scale;

			float w = static_cast<float>(ch.size.x) * scale;
			float h = static_cast<float>(ch.size.y) * scale;

			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};

			glBindTexture(GL_TEXTURE_2D, ch.texture_id);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			x += (static_cast<float>(ch.advance >> 6)) * scale;
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void TextRenderer::End() const
	{
		shader.Unbind();
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
}
