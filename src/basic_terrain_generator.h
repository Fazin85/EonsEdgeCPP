#pragma once

#include "chunk.h"
#include "abstract_level_generator.h"
#include "fast_noise.h"

class Chunk;

namespace Eon {
	class BasicTerrainGenerator : public AbstractLevelGenerator {
	public:
		void GenerateTerrainShape(ChunkPrimer& chunkPrimer, int x, int z) override;

		void DecorateChunk(Chunk& chunk) override;

	private:
		void BaseNoise(ChunkPrimer& chunk, int chunkX, int chunkZ);
		FastNoise noise_gen = FastNoise(0);
	};
}