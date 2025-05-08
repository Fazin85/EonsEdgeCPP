#include "chunk_section.h"

namespace Eon {
	Block ChunkSection::GetBlock(int x, int y, int z)
	{
		if (is_uniform) {
			return block;
		}
		else {
			if (x >= CHUNK_WIDTH || x < 0 || y >= 16 || y < 0 || z >= CHUNK_WIDTH || z < 0) {
				return { BlockType::AIR };
			}

			return blocks[x + CHUNK_WIDTH * (y + 16 * z)];
		}
	}

	void ChunkSection::SetBlock(int x, int y, int z, Block b) {
		if (x >= CHUNK_WIDTH || x < 0 || y >= 16 || y < 0 || z >= CHUNK_WIDTH || z < 0 || b.type == this->block.type) {
			return;
		}

		if (blocks.size() != CHUNK_WIDTH * 16 * CHUNK_WIDTH) {
			blocks.resize(CHUNK_WIDTH * 16 * CHUNK_WIDTH);
		}

		blocks[x + CHUNK_WIDTH * (y + 16 * z)] = b;
	}

	void ChunkSection::CalculateUniformity()
	{
		is_uniform = true;

		for (size_t i = 0; i < blocks.size(); i++) {
			if (i > 0 && blocks[i - 1].type != blocks[i].type) {
				is_uniform = false;
				break;
			}
		}

		if (is_uniform && !blocks.empty()) {
			block = blocks[0];
			std::vector<Block>().swap(blocks);
		}
	}

	bool ChunkSection::IsUniform(Block& outBlock) const
	{
		if (is_uniform) {
			outBlock = block;
		}
		else {
			outBlock = { BlockType::AIR };
		}

		return is_uniform;
	}
}
