#pragma once

#include "block.h"
#include "chunk_position.h"
#include "chunk_info.h"
#include "chunk_primer.h"
#include "chunk_section.h"

#include <array>
#include <memory>
#include <mutex>

namespace Eon
{
	class Chunk
	{
	public:
		Chunk(ChunkPrimer& chunkPrimer, ChunkPosition chunkPosition);
		void SetBlock(int x, int y, int z, Block& block);
		Block GetBlock(int x, int y, int z);
		ChunkPosition Position() const;
		short GetHeightestBlockY(int x, int z);
		void SetDecorated(bool decorated);
		bool IsDecorated() const;
		bool CanUnload() const;
		void SetCanUnload(bool canUnload);

	private:
		ChunkPosition position;
		std::array<ChunkSection, CHUNK_HEIGHT / 16> sections;
		std::unique_ptr<std::array<short, CHUNK_WIDTH* CHUNK_WIDTH>> highest_blocks;
		bool decorated;
		bool can_unload;
		std::mutex mutex;
	};
}  // namespace Eon