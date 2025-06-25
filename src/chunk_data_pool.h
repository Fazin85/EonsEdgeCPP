#pragma once

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <mutex>

#include "chunk_info.h"

namespace Eon
{
	class ChunkDataPool
	{
	public:
		explicit ChunkDataPool();

		uint8_t* Allocate();
		void Deallocate(const uint8_t* data);
		int GetAllocatedBlocks() const;

	private:
		constexpr static size_t BLOCK_SIZE = 67108864;
		constexpr static size_t ELEMENT_SIZE = CHUNK_WIDTH * 16 * CHUNK_WIDTH;
		constexpr static size_t ELEMENT_COUNT = BLOCK_SIZE / ELEMENT_SIZE;

		static std::mutex mutex;

		uint8_t* FindFreeSlot();

		std::vector<uint8_t> block;
		std::vector<bool> in_use;
		int allocated = 0;
	};
}