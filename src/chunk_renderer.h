#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <memory>

#include "chunk_mesh_construction_data.h"

namespace Eon
{
	class ChunkRenderer
	{
	public:
		ChunkRenderer(ChunkMeshConstructionData& meshConstructionData);
		~ChunkRenderer();
		void SetWaterMesh(std::unique_ptr<ChunkRenderer> waterMesh);
		void Render();
		void Setup();
		void Destroy();

	private:
		bool setup;
		unsigned int* vertex_position_data;
		unsigned int* dir_light_data;
		unsigned int* indices;
		size_t vertex_data_size;
		size_t index_size;
		std::unique_ptr<ChunkRenderer> water_mesh;
		unsigned int vao;
		unsigned int ibo;
		unsigned int vertex_position_vbo;
		unsigned int dir_light_vbo;
	};
}  // namespace Eon