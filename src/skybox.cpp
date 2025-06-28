#include <SFML/Graphics.hpp>
#include "skybox.h"
#include "glad/glad.h"
#include "log.h"
#include "gl_error_check.h"

namespace Eon
{
	Skybox::Skybox(std::array<std::string, 6>& faceFileNames)
	{
		if (shader == nullptr)
		{
			shader = std::make_unique<Shader>("Skybox.vert", "Skybox.frag");
		}

		GL_CHECK(glGenVertexArrays(1, &vao));
		GL_CHECK(glGenBuffers(1, &vbo));
		GL_CHECK(glGenBuffers(1, &ebo));
		GL_CHECK(glBindVertexArray(vao));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW));
		GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr));
		GL_CHECK(glEnableVertexAttribArray(0));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GL_CHECK(glBindVertexArray(0));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		GL_CHECK(glGenTextures(1, &texture));
		GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));
		GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

		GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

		for (size_t i = 0; i < faceFileNames.size(); i++)
		{
			std::string fileName = "content/images/" + faceFileNames[i];
			sf::Image image;
			image.loadFromFile(fileName);
			if (image.getPixelsPtr())
			{
				GL_CHECK(glTexImage2D
				(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0,
					GL_RGBA,
					image.getSize().x,
					image.getSize().y,
					0,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					image.getPixelsPtr()
				));

				EON_INFO("Loaded image: " + faceFileNames[i]);
			}
			else
			{
				EON_ERROR("Failed to load image: " + faceFileNames[i]);
			}
		}
	}

	void Skybox::Render(Camera& camera) const
	{
		GL_CHECK(glDepthFunc(GL_LEQUAL));
		GL_CHECK(glCullFace(GL_FRONT));

		camera.CalculateViewMatrix(glm::vec3(0, 0, 0));

		shader->Bind();
		shader->UniformMatrix4("view", camera.ViewMatrix());
		shader->UniformMatrix4("projection", camera.ProjectionMatrix());

		GL_CHECK(glBindVertexArray(vao));
		GL_CHECK(glActiveTexture(GL_TEXTURE0));
		GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));
		GL_CHECK(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));
		GL_CHECK(glBindVertexArray(0));

		GL_CHECK(glDepthFunc(GL_LESS));
	}
}