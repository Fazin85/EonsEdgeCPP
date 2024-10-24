#include "chunk_renderer.h"

#include <glad/glad.h>

#include "log.h"
#include <array>
#include <cmath>

namespace Eon
{
	ChunkRenderer::ChunkRenderer(ChunkMeshConstructionData& meshData) : vertex_position_data(nullptr), dir_light_data(nullptr), setup(false), water_mesh(nullptr)
	{
		vertex_data_size = meshData.vertexPositions.size();

		index_size = meshData.indices.size();
		indices = new unsigned int[index_size];
		std::memcpy(indices, meshData.indices.data(), sizeof(unsigned int) * index_size);

		vertex_position_data = new unsigned int[vertex_data_size];
		dir_light_data = new unsigned int[vertex_data_size];

		for (int i = 0; i < vertex_data_size; i++)
		{
			dir_light_data[i] = ((meshData.light[i] << 24) | (meshData.directions[i] << 16) |
				(static_cast<unsigned char>(meshData.uvs[i].x) << 8) | (static_cast<unsigned char>(meshData.uvs[i].y)));

			unsigned short y16 = static_cast<unsigned short>(meshData.vertexPositions[i].y);
			unsigned char* yy = reinterpret_cast<unsigned char*>(&y16);

			vertex_position_data[i] = ((static_cast<unsigned char>(meshData.vertexPositions[i].x) << 24) | (static_cast<unsigned char>(meshData.vertexPositions[i].z) << 16) | (yy[0] << 8) | (yy[1]));
		}
	}

	ChunkRenderer::~ChunkRenderer()
	{
		Destroy();
	}

	void ChunkRenderer::SetWaterMesh(ChunkRenderer* waterMesh)
	{
		water_mesh = waterMesh;
	}

	void ChunkRenderer::Render()
	{
		if (!setup)
		{
			return;
		}

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		glDrawElements(GL_TRIANGLES, index_size, GL_UNSIGNED_INT, 0);

		if (water_mesh != nullptr)
		{
			water_mesh->Render();
		}
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
		glBufferData(GL_ARRAY_BUFFER, vertex_data_size * sizeof(unsigned int), vertex_position_data,
			GL_STATIC_DRAW);

		glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, 0);
		glEnableVertexAttribArray(0);

		delete vertex_position_data;
		vertex_position_data = nullptr;

		glGenBuffers(1, &dir_light_vbo);

		glBindBuffer(GL_ARRAY_BUFFER, dir_light_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex_data_size * sizeof(unsigned int), dir_light_data,
			GL_STATIC_DRAW);

		glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, 0);
		glEnableVertexAttribArray(1);

		delete dir_light_data;
		dir_light_data = nullptr;

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size * sizeof(unsigned int), indices,
			GL_STATIC_DRAW);

		delete indices;
		indices = nullptr;

		if (water_mesh != nullptr)
		{
			water_mesh->Setup();
		}

		setup = true;
	}

	void ChunkRenderer::Destroy()
	{
		if (setup)
		{
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &ibo);

			glDeleteBuffers(1, &vertex_position_vbo);
			glDeleteBuffers(1, &dir_light_vbo);
		}

		if (dir_light_data != nullptr)
		{
			delete dir_light_data;
		}

		if (vertex_position_data != nullptr)
		{
			delete vertex_position_data;
		}

		if (indices != nullptr)
		{
			delete indices;
		}

		if (water_mesh != nullptr)
		{
			water_mesh->Destroy();
		}
	}

	eon_chunk_mesh_data ChunkRenderer::GetMeshDataFromGpu() const
	{
		eon_chunk_mesh_data chunkMeshData{};
		chunkMeshData.vertex_data_size = vertex_data_size;
		chunkMeshData.index_size = index_size;

		eon_get_data_from_gl_buffer_handles(&chunkMeshData, vertex_position_vbo, dir_light_vbo, ibo);

		return chunkMeshData;
	}
}  // namespace Eon
