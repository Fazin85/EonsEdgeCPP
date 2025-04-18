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
		void Render() const;
		void Setup();
		void Destroy() const;

	private:
		bool setup;
		std::vector<unsigned int> vertex_position_data;
		std::vector<unsigned int> dir_light_data;
		std::vector<unsigned int> indices;
		size_t index_count;
		unsigned int vao;
		unsigned int ibo;
		unsigned int vertex_position_vbo;
		unsigned int dir_light_vbo;
	};
}  // namespace Eon