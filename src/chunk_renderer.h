#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "chunk_mesh_construction_data.h"
#include "chunk_mesh_data.h"

namespace Eon
{
	class ChunkRenderer
	{
	public:
		ChunkRenderer(ChunkMeshConstructionData& meshConstructionData);
		~ChunkRenderer();
		void SetWaterMesh(ChunkRenderer* waterMesh);
		void Render();
		void Setup();
		void Destroy();
		eon_chunk_mesh_data GetMeshDataFromGpu() const;

	private:
		bool setup;
		unsigned int* vertex_position_data;
		unsigned int* dir_light_data;
		unsigned int* indices;
		size_t vertex_data_size;
		size_t index_size;
		ChunkRenderer* water_mesh;
		unsigned int vao;
		unsigned int ibo;
		unsigned int vertex_position_vbo;
		unsigned int dir_light_vbo;
	};
}  // namespace Eon