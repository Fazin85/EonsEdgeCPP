#include "overworld_terrain_generator.h"

namespace Eon {
	OverworldTerrainGenerator::OverworldTerrainGenerator(unsigned int seed) :
		seed(seed),
		min_limit_noise(seed, 16),
		max_limit_noise(seed + 1, 16),
		main_noise(seed + 2, 8),
		depth_noise(seed + 3, 16)
	{
		heightmap.resize(825);
		biome_weights.resize(25);
	}

	std::unique_ptr<Chunk> OverworldTerrainGenerator::GenerateTerrainShape(int x, int z)
	{
		return nullptr;
	}

	void Eon::OverworldTerrainGenerator::DecorateChunk(Chunk& chunk)
	{
	}

	void OverworldTerrainGenerator::GenerateHeightmap(int x, int y, int z)
	{
		constexpr int coordScale = 684.412f;

		depth_noise.GenerateNoiseOctaves(depth_region, x, z, 5, 5, 200.0f, 200.0f);
		main_noise.GenerateNoiseOctaves(main_noise_region, x, y, z, 5, 33, 5, coordScale / 80.0f, coordScale / 160.0f, coordScale / 80.0f);
		min_limit_noise.GenerateNoiseOctaves(min_limit_region, x, y, z, 5, 33, 5, coordScale, coordScale, coordScale);
		max_limit_noise.GenerateNoiseOctaves(max_limit_region, x, y, z, 5, 33, 5, coordScale, coordScale, coordScale);

		int i = 0;
		int j = 0;

		for (int k = 0; k < 5; k++) {
			for (int l = 0; l < 5; l++) {
				float f2 = 0.0f;
				float f3 = 0.0f;
				float f4 = 0.0f;
				int i1 = 2;

				for(int j1 = -2; j1 <= 2; j1++) {
					for (int k1 = -2; k1 <= 2; k1++) {

					}
				}
			}
		}
	}
}
