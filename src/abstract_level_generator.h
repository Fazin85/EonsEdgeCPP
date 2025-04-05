#pragma once

#include "chunk.h"
#include <memory>

namespace Eon {
	class Chunk;

	class AbstractLevelGenerator {
	public:
		virtual ~AbstractLevelGenerator() {};

		virtual std::unique_ptr<Chunk> GenerateTerrainShape(int x, int z) = 0;
		virtual void DecorateChunk(Chunk& chunk) = 0;
	};
}