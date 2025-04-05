#pragma once

#include "block.h"
#include "bounding_box.h"
#include "chunk.h"
#include "chunk_position.h"
#include "voxel_model.h"
#include "abstract_level_generator.h"

#include <array>
#include <fstream>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <parallel_hashmap/phmap.h>
#include <optional>

#define LEVEL_WIDTH_CHUNKS 32

namespace Eon
{
	class Level
	{
	public:
		Level(AbstractLevelGenerator& abstractLevelGenerator);
		~Level();
		std::optional<std::reference_wrapper<Chunk>> GetChunk(ChunkPosition position);
		std::optional<std::reference_wrapper<Chunk>> GetChunk(ChunkPosition position, bool lock);
		void SetBlock(Block block, int x, int y, int z);
		Block GetBlock(glm::ivec3 position);
		glm::vec4& SkyColor();
		std::vector<BoundingBox> GetBlockAABBs(const BoundingBox& bb);
		short GetHighestBlockY(int x, int z);
		bool ChunkExistsAt(ChunkPosition position);
		void Update(ChunkPosition playerChunkPosition, int simulationDistance);

	private:
		void GenerateAt(ChunkPosition position);
		void PlaceModel(VoxelModel& model, short x, short y, short z);
		void PlaceTree(short x, short z);
		phmap::parallel_node_hash_map<ChunkPosition, std::unique_ptr<Chunk>> chunks;
		glm::vec4 sky_color;
		AbstractLevelGenerator& abstract_level_generator;
		std::unique_ptr<VoxelModel> tree_model;
		std::mutex chunk_mutex;
	};
} // namespace Eon