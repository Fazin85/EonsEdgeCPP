#include "basic_terrain_generator.h"
#include <algorithm>

namespace Eon {
	std::unique_ptr<Chunk> BasicTerrainGenerator::GenerateTerrainShape(int x, int z)
	{
		auto chunk = std::make_unique<Chunk>(ChunkPosition(x, z));

		BaseNoise(*chunk);

		return std::move(chunk);
	}

	void BasicTerrainGenerator::DecorateChunk(Chunk& chunk)
	{
	}

	static float AverageHeight(float baseLevel, float input) {
		return ((baseLevel + input) / 2) + (baseLevel / 2);
	}

	static float LinearInterpolate3D(float xm_ym_zm, float xp_ym_zm, float xm_yp_zm, float xp_yp_zm,
		float xm_ym_zp, float xp_ym_zp, float xm_yp_zp, float xp_yp_zp,
		float x, float y, float z)
	{
		return (xm_ym_zm * (1 - x) * (1 - y) * (1 - z)) + (xp_ym_zm * x * (1 - y) * (1 - z)) + (xm_yp_zm * (1 - x) * y * (1 - z)) + (xp_yp_zm * x * y * (1 - z)) +
			(xm_ym_zp * (1 - x) * (1 - y) * z) + (xp_ym_zp * x * (1 - y) * z) + (xm_yp_zp * (1 - x) * y * z) + (xp_yp_zp * x * y * z);
	}

	void BasicTerrainGenerator::BaseNoise(Chunk& chunk)
	{
		const int waterLevel = 84;
		const int noiseDivisorHorizontal = 4;
		const int noiseDivisorVertical = 4;
		const int noiseMapWidth = (CHUNK_WIDTH / noiseDivisorHorizontal) + 2;
		const int noiseMapHeight = (CHUNK_HEIGHT / noiseDivisorVertical) + 2;

		float noiseValues[noiseMapWidth][noiseMapHeight][noiseMapWidth]{};

		noise_gen.SetFractalOctaves(8);

		for (int x = 0; x < noiseMapWidth; x++) {
			for (int y = 0; y < noiseMapHeight; y++) {
				for (int z = 0; z < noiseMapWidth; z++) {
					noiseValues[x][y][z] = noise_gen.GetSimplexFractal(chunk.Position().x + (x * noiseDivisorHorizontal), y * noiseDivisorVertical, chunk.Position().z + (z * noiseDivisorHorizontal));
				}
			}
		}

		for (int x = 0; x < CHUNK_WIDTH; x++) {
			for (int z = 0; z < CHUNK_WIDTH; z++) {
				int worldX = chunk.Position().x + x;
				int worldZ = chunk.Position().z + z;

				float squishFactor = 0.1f;
				squishFactor -= noise_gen.GetSimplexFractal(worldX * 0.1f, worldZ * 0.1f) / 10 * 3;
				squishFactor = std::clamp(squishFactor, 0.01f, 0.15f);

				float cont = noise_gen.GetSimplex(worldX * 0.1f, worldZ * 0.1f) * waterLevel;
				float mountain = noise_gen.GetSimplexFractal(worldX * 0.4f, worldZ * 0.4f) + 1 * 16;
				cont += mountain;

				for (int y = 0; y < CHUNK_HEIGHT; y++) {
					float baseLevel = AverageHeight(waterLevel, cont);
					float densityMod = (baseLevel - y) * squishFactor;

					int x2 = (x / noiseDivisorHorizontal) + 1;
					int y2 = (y / noiseDivisorVertical) + 1;
					int z2 = (z / noiseDivisorHorizontal) + 1;

					float density = LinearInterpolate3D(noiseValues[x2][y2][z2], noiseValues[x2 + 1][y2][z2],
						noiseValues[x2][y2 + 1][z2], noiseValues[x2 + 1][y2 + 1][z2],
						noiseValues[x2][y2][z2 + 1], noiseValues[x2 + 1][y2][z2 + 1],
						noiseValues[x2][y2 + 1][z2 + 1], noiseValues[x2 + 1][y2 + 1][z2 + 1],
						x % noiseDivisorHorizontal / (float)noiseDivisorHorizontal,
						y % noiseDivisorVertical / (float)noiseDivisorVertical,
						z % noiseDivisorHorizontal / (float)noiseDivisorHorizontal);

					Block block;

					if (density + densityMod >= 0.0f) {
						block.type = BlockType::STONE;
					}
					else {
						block.type = BlockType::AIR;

						if (y <= waterLevel) {
							block.type = BlockType::WATER;
						}
					}

					chunk.SetBlock(x, y, z, block);
				}
			}
		}
	}
}
