#include "chunk_primer.h"

namespace Eon
{
	const Block& ChunkPrimer::GetBlock(int x, int y, int z) const
	{
		if (x >= CHUNK_WIDTH || x < 0 || y >= CHUNK_HEIGHT || y < 0 || z >= CHUNK_WIDTH || z < 0)
		{
			return BlockRegistry::GetBlockByID(0);
		}

		return BlockRegistry::GetBlockByID(blocks[x][y][z]);
	}

	void ChunkPrimer::SetBlock(int x, int y, int z, const Block& block)
	{
		if (x >= CHUNK_WIDTH || x < 0 || y >= CHUNK_HEIGHT || y < 0 || z >= CHUNK_WIDTH || z < 0)
		{
			return;
		}

		blocks[x][y][z] = block.GetID();
	}
}
