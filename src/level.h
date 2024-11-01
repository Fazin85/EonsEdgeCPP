#pragma once

#include "block.h"
#include "bounding_box.h"
#include "chunk.h"
#include "chunk_position.h"
#include "voxel_model.h"
#include <array>
#include <fstream>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>

#define LEVEL_WIDTH_CHUNKS 32

namespace Eon
{
	class Level
	{
	public:
		Level();
		~Level();
		Chunk* GetChunk(ChunkPosition position);
		Chunk* GetChunkUnsafe(ChunkPosition position);
		Chunk* GetChunkFromIndex(size_t index);
		Block* GetBlock(short x, short y, short z);
		void SetBlock(Block block, short x, short y, short z);
		Block* GetBlock(glm::ivec3 position);
		glm::vec4& SkyColor();
		std::vector<BoundingBox> GetBlockAABBs(const BoundingBox& bb);
		unsigned int IndexFromPosition(short x, short z);
		short GetHighestBlockY(short x, short z);

	private:
		Block* GetBlockDecompressChunk(glm::ivec3 position);
		void PlaceModel(VoxelModel& model, short x, short y, short z);
		void LoadChunkDataFromFilesystem(Chunk& chunk);
		void PlaceTree(short x, short z);
		std::array<std::unique_ptr<Chunk>, LEVEL_WIDTH_CHUNKS* LEVEL_WIDTH_CHUNKS> chunks;
		std::array<std::unique_ptr<std::fstream>, LEVEL_WIDTH_CHUNKS* LEVEL_WIDTH_CHUNKS> chunks_files;
		glm::vec4 sky_color;
		std::fstream mesh_save_file;
		std::unique_ptr<VoxelModel> tree_model;
	};
} // namespace Eon