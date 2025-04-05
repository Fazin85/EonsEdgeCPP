#pragma once

#include "block.h"
#include "chunk_position.h"
#include "chunk_info.h"

#include <array>
#include <memory>
#include <mutex>

namespace Eon
{
	class Chunk
	{
	public:
		Chunk(ChunkPosition chunkPosition);
		void SetBlock(int x, int y, int z, Block& block);
		Block GetBlock(int x, int y, int z);
		ChunkPosition Position() const;
		std::array<Block, CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_WIDTH>& GetBlocks();
		short GetHeightestBlockY(int x, int z);
		void SetDecorated(bool decorated);
		bool IsDecorated() const;

	private:
		ChunkPosition position;
		std::unique_ptr<std::array<Block, CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_WIDTH>> blocks;
		std::unique_ptr<std::array<short, CHUNK_WIDTH* CHUNK_WIDTH>> highest_blocks;
		bool decorated;
		std::mutex mutex;
	};
}  // namespace Eon