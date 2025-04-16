#pragma once

#include "chunk.h"
#include "abstract_level_generator.h"
#include "fast_noise.h"

class Chunk;

namespace Eon {
	class BasicTerrainGenerator : public AbstractLevelGenerator {
	public:
		std::unique_ptr<Chunk> GenerateTerrainShape(int x, int z) override;

		void DecorateChunk(Chunk& chunk) override;

	private:
		void BaseNoise(Chunk& chunk);
		FastNoise noise_gen = FastNoise(0);
	};
}