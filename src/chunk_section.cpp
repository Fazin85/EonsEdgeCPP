#include "chunk_section.h"

namespace Eon
{
	const Block& ChunkSection::GetBlock(int x, int y, int z)
	{
		if (is_uniform)
		{
			return BlockRegistry::GetBlockByID(block);
		}
		else
		{
			if (x >= CHUNK_WIDTH || x < 0 || y >= 16 || y < 0 || z >= CHUNK_WIDTH || z < 0)
			{
				return BlockRegistry::GetBlockByID(0);
			}

			return BlockRegistry::GetBlockByID(blocks[x + CHUNK_WIDTH * (y + 16 * z)]);
		}
	}

	void ChunkSection::SetBlock(int x, int y, int z, const Block& b)
	{
		if (x >= CHUNK_WIDTH || x < 0 || y >= 16 || y < 0 || z >= CHUNK_WIDTH || z < 0 || b.GetID() == block)
		{
			return;
		}

		if (blocks.size() != CHUNK_WIDTH * 16 * CHUNK_WIDTH)
		{
			blocks.resize(CHUNK_WIDTH * 16 * CHUNK_WIDTH);
		}

		blocks[x + CHUNK_WIDTH * (y + 16 * z)] = b.GetID();
	}

	void ChunkSection::CalculateUniformity()
	{
		is_uniform = true;

		for (size_t i = 0; i < blocks.size(); i++)
		{
			if (i > 0 && blocks[i - 1] != blocks[i])
			{
				is_uniform = false;
				break;
			}
		}

		if (is_uniform && !blocks.empty())
		{
			block = blocks[0];
			std::vector<uint8_t>().swap(blocks);
		}
	}

	std::optional<std::reference_wrapper<const Block>> ChunkSection::IsUniform() const
	{
		if (is_uniform)
		{
			return BlockRegistry::GetBlockByID(block);
		}
		else
		{
			return std::nullopt;
		}
	}
}
