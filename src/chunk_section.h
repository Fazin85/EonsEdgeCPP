#pragma once

#include "block.h"
#include "chunk_info.h"

#include <vector>

namespace Eon {
	class ChunkSection {
	public:
		Block GetBlock(int x, int y, int z);
		void SetBlock(int x, int y, int z, Block block);

		void CalculateUniformity();

	private:
		bool is_uniform = true;
		Block block;

		std::vector<Block> blocks;
	};
}