#pragma once

#include "block.h"
#include "chunk_position.h"

namespace Eon {
	class ChunkPrimer {
	public:
		Block GetBlock(int x, int y, int z) const;
		void SetBlock(int x, int y, int z, Block block);

	private:
		Block blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_WIDTH]{};
	};
}