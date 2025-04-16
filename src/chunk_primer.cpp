#include "chunk_primer.h"

namespace Eon {
	Block ChunkPrimer::GetBlock(int x, int y, int z) const
	{
		if (x >= CHUNK_WIDTH || x < 0 || y >= CHUNK_HEIGHT || y < 0 || z >= CHUNK_WIDTH || z < 0) {
			return { BlockType::AIR };
		}

		return blocks[x][y][z];
	}

	void ChunkPrimer::SetBlock(int x, int y, int z, Block block)
	{
		if (x >= CHUNK_WIDTH || x < 0 || y >= CHUNK_HEIGHT || y < 0 || z >= CHUNK_WIDTH || z < 0) {
			return;
		}

		blocks[x][y][z] = block;
	}
}
