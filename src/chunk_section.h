#pragma once

#include "block.h"
#include "chunk_info.h"
#include "chunk_data_pool.h"

#include <vector>

namespace Eon
{
	class ChunkSection
	{
	public:
		static ChunkDataPool chunk_data_pool;

		~ChunkSection();

		const Block& GetBlock(int x, int y, int z) const;
		void SetBlock(int x, int y, int z, const Block& block);

		void CalculateUniformity();
		std::optional<std::reference_wrapper<const Block>> IsUniform() const;

	private:
		
		bool is_uniform = true;
		uint8_t block = 0;
		uint8_t* blocks = nullptr;
		int blocks_size = 0;
	};
}