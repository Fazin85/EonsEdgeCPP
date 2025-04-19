#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <memory>
#include <optional>

#include "chunk_mesh_construction_data.h"
#include "display_list.h"

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
		std::vector<glm::ivec3> vertices;
		std::vector<unsigned int> indices;
		int list;
	};
}  // namespace Eon