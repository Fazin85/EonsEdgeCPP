#include "basic_terrain_generator.h"
#include "fast_noise_lite.h"

std::unique_ptr<Eon::Chunk> Eon::BasicTerrainGenerator::GenerateTerrainShape(int x, int z)
{
	auto chunk = std::make_unique<Chunk>(ChunkPosition(x, z));

	FastNoiseLite fn{};
	Block stone{ BlockType::STONE };

	for (int bx = 0; bx < CHUNK_WIDTH; bx++) {
		for (int bz = 0; bz < CHUNK_WIDTH; bz++) {
			float noise = (fn.GetNoise(static_cast<float>(x + bx) * 0.5f, static_cast<float>(z + bz) * 0.5f) + 1.0f) / 2.0f;

			int height = (int)(noise * 40.0f);

			for (int y = 0; y < height; y++) {
				chunk->SetBlock(bx, y, bz, stone);
			}
		}
	}

	return std::move(chunk);
}

void Eon::BasicTerrainGenerator::DecorateChunk(Chunk& chunk)
{
}
