#include "mesh.h"
#include "log.h"
#include "gl_error_check.h"

#include <stdexcept>

namespace Eon
{
	PositionTextureColorNormalMesh::PositionTextureColorNormalMesh(
		std::vector<glm::vec3, PoolAllocator<glm::vec3>>&& positions,
		std::vector<glm::vec2, PoolAllocator<glm::vec2>>&& texture_coords,
		std::vector<glm::vec3, PoolAllocator<glm::vec3>>&& colors,
		std::vector<glm::vec3, PoolAllocator<glm::vec3>>&& normals) :
		positions(std::move(positions)),
		texture_coords(std::move(texture_coords)),
		colors(std::move(colors)),
		normals(std::move(normals))
	{
		std::vector<size_t> sizes = { this->positions.size(), this->texture_coords.size(), this->colors.size(), this->normals.size() };
		vertex_count = CalculateVertexCount(sizes);
	}

	PositionTextureColorNormalMesh::~PositionTextureColorNormalMesh()
	{
		if (!IsSetup())
		{
			return;
		}

		GL_CHECK(glDeleteVertexArrays(1, &vao));
		GL_CHECK(glDeleteBuffers(1, &position_vbo));
		GL_CHECK(glDeleteBuffers(1, &texture_coords_vbo));
		GL_CHECK(glDeleteBuffers(1, &color_vbo));
		GL_CHECK(glDeleteBuffers(1, &normal_vbo));
	}

	void PositionTextureColorNormalMesh::Setup()
	{
		if (IsSetup())
		{
			EON_WARN("Tried to setup mesh multiple times");
			return;
		}

		GenVBO(&position_vbo, positions);
		GenVBO(&texture_coords_vbo, texture_coords);
		GenVBO(&color_vbo, colors);
		GenVBO(&normal_vbo, normals);

		GL_CHECK(glGenVertexArrays(1, &vao));
		GL_CHECK(glBindVertexArray(vao));

		{
			using enum AttributeLocation;
			SetupVBO(position_vbo, static_cast<int>(Position), 3, GL_FLOAT);
			SetupVBO(texture_coords_vbo, static_cast<int>(TextureCoord), 2, GL_FLOAT);
			SetupVBO(color_vbo, static_cast<int>(Color), 3, GL_FLOAT);
			SetupVBO(normal_vbo, static_cast<int>(Normal), 3, GL_FLOAT);
		}

		EndSetup();
	}

	void Mesh::Render() const
	{
		if (!IsSetup())
		{
			return;
		}

		GL_CHECK(glBindVertexArray(vao));
		GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, vertex_count));
	}

	bool Mesh::IsSetup() const
	{
		return setup;
	}

	void Mesh::SetupVBO(GLuint vbo, int location, int size, int type) const
	{
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		GL_CHECK(glVertexAttribPointer(location, size, type, false, 0, nullptr));
		GL_CHECK(glEnableVertexAttribArray(location));
	}

	GLsizei Mesh::CalculateVertexCount(const std::vector<size_t>& sizes) const
	{
		if (sizes.empty())
		{
			throw std::runtime_error("Vertex data is empty!");
		}

		for (size_t i = 0; i < sizes.size(); i++)
		{
			if (i > 0 && sizes[i] != sizes[i - 1])
			{
				throw std::runtime_error("Vertex data is not correct!");
			}
		}

		return static_cast<GLsizei>(sizes[0]);
	}

	void Mesh::EndSetup()
	{
		GL_CHECK(glBindVertexArray(0));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

		setup = true;
	}

	PositionTextureMesh::PositionTextureMesh(
		std::vector<glm::vec3, PoolAllocator<glm::vec3>>&& positions,
		std::vector<glm::vec2, PoolAllocator<glm::vec2>>&& textureCoords) : positions(std::move(positions)), texture_coords(std::move(textureCoords))
	{
		std::vector<size_t> sizes = { this->positions.size(), this->texture_coords.size(), };
		vertex_count = CalculateVertexCount(sizes);
	}

	PositionTextureMesh::~PositionTextureMesh()
	{
		if (!IsSetup())
		{
			return;
		}

		GL_CHECK(glDeleteVertexArrays(1, &vao));
		GL_CHECK(glDeleteBuffers(1, &position_vbo));
		GL_CHECK(glDeleteBuffers(1, &texture_coords_vbo));
	}

	void PositionTextureMesh::Setup()
	{
		if (IsSetup())
		{
			EON_WARN("Tried to setup mesh multiple times");
			return;
		}

		GenVBO(&position_vbo, positions);
		GenVBO(&texture_coords_vbo, texture_coords);

		GL_CHECK(glGenVertexArrays(1, &vao));
		GL_CHECK(glBindVertexArray(vao));

		{
			using enum AttributeLocation;
			SetupVBO(position_vbo, static_cast<int>(Position), 3, GL_FLOAT);
			SetupVBO(texture_coords_vbo, static_cast<int>(TextureCoord), 2, GL_FLOAT);
		}

		EndSetup();
	}
}
