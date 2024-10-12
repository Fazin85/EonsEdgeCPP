#include "chunk_renderer.h"

#include <glad/glad.h>

#include <array>
#include <cmath>

namespace Eon
{
	ChunkRenderer::ChunkRenderer(Chunk*, ChunkMeshData& meshData)
	{
		this->chunk = chunk;
		this->ready = false;
		this->setup = false;
		this->indices = indices;
		this->water_mesh = nullptr;

		vertex_position_data.reserve(meshData.vertexPositions.size());
		dir_light_data.reserve(meshData.vertexPositions.size());

		for (int i = 0; i < meshData.vertexPositions.size(); i++)
		{
			u8 texX = static_cast<u8>(
				std::floorf(meshData.uvs[i].x >= 1.0f ? 255.0f : meshData.uvs[i].x * 256.0f));
			u8 texY = static_cast<u8>(
				std::floorf(meshData.uvs[i].y >= 1.0f ? 255.0f : meshData.uvs[i].y * 256.0f));

			vertex_position_data.push_back((meshData.light[i] << 24) | (meshData.directions[i] << 16) |
				(texX << 8) | (texY));

			u8 px = static_cast<u8>(meshData.vertexPositions[i].x);
			u8 pz = static_cast<u8>(meshData.vertexPositions[i].z);
			u16 y16 = static_cast<u16>(meshData.vertexPositions[i].y);

			u8* yy = reinterpret_cast<u8*>(&y16);

			dir_light_data.push_back((px << 24) | (pz << 16) | (yy[0] << 8) | (yy[1]));
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

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
			GL_UNSIGNED_INT, 0);

		if (water_mesh != nullptr)
		{
			water_mesh->Render();
		}
	}

	void ChunkRenderer::Setup()
	{
		if (ready)
		{
			return;
		}

		setup = false;

		vao.Init();
		ibo.Init(indices);

		dir_light_vbo.Init(dir_light_data);
		vao.Link(1, 1, dir_light_vbo, GL_FLOAT, false);

		vertex_position_vbo.Init(vertex_position_data);
		vao.Link(0, 1, vertex_position_vbo, GL_UNSIGNED_INT, true);

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

		if (water_mesh != nullptr)
		{
			water_mesh->Destroy();
		}
	}
}  // namespace Eon
