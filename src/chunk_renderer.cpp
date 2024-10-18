#include "chunk_renderer.h"

#include <glad/glad.h>

#include "log.h"
#include <array>
#include <cmath>

namespace Eon
{
	ChunkRenderer::ChunkRenderer(Chunk* chunk, ChunkMeshData& meshData) : aabb(glm::vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)), vertex_position_data(nullptr), dir_light_data(nullptr)
	{
		this->chunk = chunk;
		this->setup = false;
		this->water_mesh = nullptr;

		ibo = IndexBufferObject();
		aabb.Update(glm::vec3(chunk->Position().x * CHUNK_WIDTH, 0, chunk->Position().z * CHUNK_WIDTH));

		vertex_data_size = meshData.vertexPositions.size();

		index_size = meshData.indices.size();
		indices = new u32[index_size];
		std::memcpy(indices, meshData.indices.data(), sizeof(u32) * index_size);

		vertex_position_data = new u32[vertex_data_size];
		dir_light_data = new u32[vertex_data_size];

		for (int i = 0; i < vertex_data_size; i++)
		{
			u8 texX = static_cast<u8>(
				std::floorf(meshData.uvs[i].x >= 1.0f ? 255.0f : meshData.uvs[i].x * 256.0f));
			u8 texY = static_cast<u8>(
				std::floorf(meshData.uvs[i].y >= 1.0f ? 255.0f : meshData.uvs[i].y * 256.0f));

			dir_light_data[i] = ((meshData.light[i] << 24) | (meshData.directions[i] << 16) |
				(texX << 8) | (texY));

			u8 px = static_cast<u8>(meshData.vertexPositions[i].x);
			u8 pz = static_cast<u8>(meshData.vertexPositions[i].z);
			u16 y16 = static_cast<u16>(meshData.vertexPositions[i].y);

			u8* yy = reinterpret_cast<u8*>(&y16);

			vertex_position_data[i] = ((px << 24) | (pz << 16) | (yy[0] << 8) | (yy[1]));
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

	AABB& ChunkRenderer::GetAABB()
	{
		return aabb;
	}

	Chunk& ChunkRenderer::GetChunk()
	{
		return *chunk;
	}
}  // namespace Eon
