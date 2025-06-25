#include "chunk_section.h"

namespace Eon
{
	ChunkDataPool ChunkSection::chunk_data_pool;

	ChunkSection::~ChunkSection()
	{
		if (blocks_size > 0 && blocks != nullptr)
		{
			chunk_data_pool.Deallocate(blocks);
		}
	}

	const Block& ChunkSection::GetBlock(int x, int y, int z) const
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

		if (blocks_size != CHUNK_WIDTH * 16 * CHUNK_WIDTH && blocks == nullptr)
		{
			blocks = chunk_data_pool.Allocate();
			blocks_size = CHUNK_WIDTH * 16 * CHUNK_WIDTH;
		}

		blocks[x + CHUNK_WIDTH * (y + 16 * z)] = b.GetID();
	}

	void ChunkSection::CalculateUniformity()
	{
		is_uniform = true;

		for (size_t i = 0; i < blocks_size; i++)
		{
			if (i > 0 && blocks[i - 1] != blocks[i])
			{
				is_uniform = false;
				break;
			}
		}

		if (is_uniform && blocks_size > 0 && blocks != nullptr)
		{
			block = blocks[0];

			chunk_data_pool.Deallocate(blocks);
			blocks = nullptr;
			blocks_size = 0;
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
