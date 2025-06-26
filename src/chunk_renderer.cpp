#include "chunk_renderer.h"

#include <glad/glad.h>

#include "log.h"
#include <array>
#include <cmath>

namespace Eon
{
	ChunkRenderer::ChunkRenderer(ChunkMeshConstructionData& meshData) :
		vertex_position_data(PoolAllocators::u32_allocator),
		dir_light_data(PoolAllocators::u32_allocator),
		indices(PoolAllocators::u32_allocator),
		setup(false)
	{
		indices = meshData.indices;
		index_count = indices.size();

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

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(index_count), GL_UNSIGNED_INT, nullptr);
	}

	void ChunkRenderer::Setup()
	{
		if (setup)
		{
			EON_WARN("Tried to setup chunk renderer multiple times");
			return;
		}

		setup = false;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vertex_position_vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vertex_position_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex_position_data.size() * sizeof(unsigned int), vertex_position_data.data(),
			GL_STATIC_DRAW);

		vertex_position_data.clear();
		vertex_position_data.shrink_to_fit();

		glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &dir_light_vbo);

		glBindBuffer(GL_ARRAY_BUFFER, dir_light_vbo);
		glBufferData(GL_ARRAY_BUFFER, dir_light_data.size() * sizeof(unsigned int), dir_light_data.data(),
			GL_STATIC_DRAW);

		dir_light_data.clear();
		dir_light_data.shrink_to_fit();

		glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, nullptr);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
			GL_STATIC_DRAW);

		indices.clear();
		indices.shrink_to_fit();

		setup = true;
	}

	void ChunkRenderer::Destroy() const
	{
		if (setup)
		{
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &ibo);

			glDeleteBuffers(1, &vertex_position_vbo);
			glDeleteBuffers(1, &dir_light_vbo);
		}
	}
}  // namespace Eon
