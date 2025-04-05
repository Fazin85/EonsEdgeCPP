#pragma once

#include "chunk.h"
#include "abstract_level_generator.h"

class Chunk;

namespace Eon {
	class BasicTerrainGenerator : public AbstractLevelGenerator {
	public:
		std::unique_ptr<Chunk> GenerateTerrainShape(int x, int z) override;

		void DecorateChunk(Chunk& chunk) override;
	};
}