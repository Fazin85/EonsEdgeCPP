#pragma once

#include "block.h"
#include "chunk_info.h"

#include <vector>

namespace Eon {
	class ChunkSection {
	public:
		const Block& GetBlock(int x, int y, int z);
		void SetBlock(int x, int y, int z, const Block& block);

		void CalculateUniformity();
		std::optional<std::reference_wrapper<const Block>> IsUniform() const;

	private:
		bool is_uniform = true;
		uint8_t block;

		std::vector<uint8_t> blocks;
	};
}