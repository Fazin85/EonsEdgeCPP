#include "chunk_renderer.h"

#include <glad/glad.h>

#include "log.h"
#include <array>
#include <cmath>

namespace Eon
{
	ChunkRenderer::ChunkRenderer(ChunkMeshData& meshData) : vertex_position_data(nullptr), dir_light_data(nullptr)
	{
		this->setup = false;
		this->water_mesh = nullptr;

		ibo = IndexBufferObject();

		vertex_data_size = meshData.vertexPositions.size();

		index_size = meshData.indices.size();
		indices = new u32[index_size];
		std::memcpy(indices, meshData.indices.data(), sizeof(u32) * index_size);

		vertex_position_data = new u32[vertex_data_size];
		dir_light_data = new u32[vertex_data_size];

		for (int i = 0; i < vertex_data_size; i++)
		{
			dir_light_data[i] = ((meshData.light[i] << 24) | (meshData.directions[i] << 16) |
				(static_cast<u8>(meshData.uvs[i].x) << 8) | (static_cast<u8>(meshData.uvs[i].y)));

			u16 y16 = static_cast<u16>(meshData.vertexPositions[i].y);
			u8* yy = reinterpret_cast<u8*>(&y16);

			vertex_position_data[i] = ((static_cast<u8>(meshData.vertexPositions[i].x) << 24) | (static_cast<u8>(meshData.vertexPositions[i].z) << 16) | (yy[0] << 8) | (yy[1]));
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

		vao.Bind();
		ibo.Bind();

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

		vao.Init();
		vao.Bind();

		vertex_position_vbo.Init(vertex_position_data, vertex_data_size);
		vao.Link(0, 1, vertex_position_vbo, GL_UNSIGNED_INT, true);

		delete vertex_position_data;
		vertex_position_data = nullptr;

		dir_light_vbo.Init(dir_light_data, vertex_data_size);
		vao.Link(1, 1, dir_light_vbo, GL_UNSIGNED_INT, true);

		delete dir_light_data;
		dir_light_data = nullptr;

		ibo.Init(indices, index_size);

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
			vao.Destroy();
			ibo.Destroy();

			vertex_position_vbo.Destroy();
			dir_light_vbo.Destroy();
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
}  // namespace Eon
