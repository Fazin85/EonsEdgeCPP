#pragma once

#include "block.h"
#include "bounding_box.h"
#include "chunk.h"
#include "chunk_position.h"

#include <array>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <fstream>

#define LEVEL_WIDTH_CHUNKS 40

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
		Block* GetBlock(glm::ivec3 position);
		glm::vec4& SkyColor();
		std::vector<BoundingBox> GetBlockAABBs(const BoundingBox& bb);
		unsigned int IndexFromPosition(short x, short z);

	private:
		Block* GetBlockDecompressChunk(glm::ivec3 position);
		void LoadChunkDataFromFilesystem(Chunk& chunk);
		std::array<std::unique_ptr<Chunk>, LEVEL_WIDTH_CHUNKS* LEVEL_WIDTH_CHUNKS> chunks;
		std::array<std::unique_ptr<std::fstream>, LEVEL_WIDTH_CHUNKS* LEVEL_WIDTH_CHUNKS> chunks_files;
		glm::vec4 sky_color;
		std::fstream mesh_save_file;
	};
} // namespace Eon