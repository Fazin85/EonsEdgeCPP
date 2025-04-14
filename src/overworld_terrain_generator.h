#pragma once

#include "abstract_level_generator.h"
#include "noise_generator_octaves.h"

namespace Eon {
	class OverworldTerrainGenerator : public AbstractLevelGenerator {
	public:
		OverworldTerrainGenerator(unsigned int seed);

		std::unique_ptr<Chunk> GenerateTerrainShape(int x, int z) override;

		void DecorateChunk(Chunk& chunk) override;

	private:
		void GenerateHeightmap(int x, int y, int z);
		
		NoiseGeneratorOctaves min_limit_noise;
		NoiseGeneratorOctaves max_limit_noise;
		NoiseGeneratorOctaves main_noise;

		NoiseGeneratorOctaves depth_noise;

		std::vector<double> main_noise_region;
		std::vector<double> min_limit_region;
		std::vector<double> max_limit_region;
		std::vector<double> depth_region;

		std::vector<double> heightmap;
		std::vector<float> biome_weights;

		unsigned int seed;
	};
}