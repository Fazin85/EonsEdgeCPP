#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <memory>
#include <optional>

#include "chunk_mesh_construction_data.h"

namespace Eon
{
	class ChunkRenderer
	{
	public:
		ChunkRenderer(ChunkMeshConstructionData& meshConstructionData);
		~ChunkRenderer();
		void SetWaterMesh(std::unique_ptr<ChunkRenderer> waterMesh);
		void Render() const;
		void Setup();
		void Destroy();
		std::optional<std::reference_wrapper<ChunkRenderer>> GetWaterMesh();

	private:
		bool setup;
		std::vector<unsigned int> vertex_position_data;
		std::vector<unsigned int> dir_light_data;
		std::vector<unsigned int> indices;
		size_t index_count;
		std::optional<std::unique_ptr<ChunkRenderer>> water_mesh;
		unsigned int vao;
		unsigned int ibo;
		unsigned int vertex_position_vbo;
		unsigned int dir_light_vbo;
	};
}  // namespace Eon