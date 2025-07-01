#include "chunk_renderer.h"

#include <glad/glad.h>

#include "log.h"
#include "gl_error_check.h"
#include <array>
#include <cmath>

namespace Eon
{
	ChunkRenderer::ChunkRenderer(ChunkMeshConstructionData& meshData) :
		vertex_position_data(*PoolAllocators::GetInstance().u32_allocator),
		dir_light_data(*PoolAllocators::GetInstance().u32_allocator),
		setup(false)
	{
		vertex_count = meshData.vertexPositions.size();

		vertex_position_data.resize(meshData.vertexPositions.size());
		dir_light_data.resize(meshData.vertexPositions.size());

		for (int i = 0; i < meshData.vertexPositions.size(); i++)
		{
			dir_light_data[i] = ((meshData.light[i] << 24) | (meshData.directions[i] << 16) |
				(static_cast<unsigned char>(meshData.uvs[i].x) << 8) | (static_cast<unsigned char>(meshData.uvs[i].y)));

			const auto y16 = static_cast<const unsigned short>(meshData.vertexPositions[i].y);
			const auto yy = reinterpret_cast<const unsigned char*>(&y16);

			vertex_position_data[i] = ((static_cast<unsigned char>(meshData.vertexPositions[i].x) << 24) | (static_cast<unsigned char>(meshData.vertexPositions[i].z) << 16) | (yy[0] << 8) | (yy[1]));
		}
	}

	ChunkRenderer::~ChunkRenderer()
	{
		Destroy();
	}

	void ChunkRenderer::Render() const
	{
		if (!setup)
		{
			return;
		}

		GL_CHECK(glBindVertexArray(vao));
		GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertex_count)));
	}

	void ChunkRenderer::Setup()
	{
		if (setup)
		{
			EON_WARN("Tried to setup chunk renderer multiple times");
			return;
		}

		setup = false;

		GL_CHECK(glGenVertexArrays(1, &vao));
		GL_CHECK(glBindVertexArray(vao));
		GL_CHECK(glGenBuffers(1, &vertex_position_vbo));

		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertex_position_vbo));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertex_position_data.size() * sizeof(unsigned int), vertex_position_data.data(),
			GL_STATIC_DRAW));

		vertex_position_data.clear();
		vertex_position_data.shrink_to_fit();

		GL_CHECK(glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, nullptr));
		GL_CHECK(glEnableVertexAttribArray(0));

		GL_CHECK(glGenBuffers(1, &dir_light_vbo));

		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, dir_light_vbo));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, dir_light_data.size() * sizeof(unsigned int), dir_light_data.data(),
			GL_STATIC_DRAW));

		dir_light_data.clear();
		dir_light_data.shrink_to_fit();

		GL_CHECK(glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, nullptr));
		GL_CHECK(glEnableVertexAttribArray(1));

		setup = true;
	}

	void ChunkRenderer::Destroy() const
	{
		if (setup)
		{
			GL_CHECK(glDeleteVertexArrays(1, &vao));

			GL_CHECK(glDeleteBuffers(1, &vertex_position_vbo));
			GL_CHECK(glDeleteBuffers(1, &dir_light_vbo));
		}
	}
}  // namespace Eon
