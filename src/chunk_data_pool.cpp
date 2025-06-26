#include "chunk_data_pool.h"

namespace Eon
{
	std::mutex ChunkDataPool::mutex;

	ChunkDataPool::ChunkDataPool()
	{
		block.resize(BLOCK_SIZE, 0);
		in_use.resize(ELEMENT_COUNT, false);
	}

	uint8_t* ChunkDataPool::Allocate()
	{
		{
			std::scoped_lock<std::mutex> lock(mutex);

			if (uint8_t* mem = FindFreeSlot(); mem)
			{
				std::memset(mem, 0, ELEMENT_SIZE);
				allocated++;
				return mem;
			}
		}

		return nullptr;
	}

	void ChunkDataPool::Deallocate(const uint8_t* data)
	{
		std::scoped_lock<std::mutex> lock(mutex);

		if (!data || data < &block[0] || data > &block[block.size() - 1])
		{
			throw std::exception("Invalid pointer");
		}

		ptrdiff_t offset = data - &block[0];
		if (offset % ELEMENT_SIZE != 0)
		{
			throw std::exception("Pointer not aligned to element boundary");
		}

		size_t index = offset / ELEMENT_SIZE;
		if (index < ELEMENT_COUNT)
		{
			in_use[index] = false;
			allocated--;
		}
	}

	int ChunkDataPool::GetAllocatedBlocks() const
	{
		std::scoped_lock<std::mutex> lock(mutex);
		return allocated;
	}

	uint8_t* ChunkDataPool::FindFreeSlot()
	{
		for (int i = 0; i < ELEMENT_COUNT; i++)
		{
			if (!in_use[i])
			{
				in_use[i] = true;
				return block.data() + (i * ELEMENT_SIZE);
			}
		}

		return nullptr;
	}
}