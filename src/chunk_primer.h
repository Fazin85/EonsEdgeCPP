#pragma once

#include "block.h"
#include "chunk_position.h"

namespace Eon {
	class ChunkPrimer {
	public:
		const Block& GetBlock(int x, int y, int z) const;
		void SetBlock(int x, int y, int z, const Block& block);

	private:
		uint8_t blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_WIDTH]{};
	};
}