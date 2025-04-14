#pragma once

#include "noise_generator_improved.h"
#include <vector>
#include <random>
#include <memory>

namespace Eon {
	class NoiseGeneratorOctaves {
	private:
		/**
		 * Collection of noise generation functions. Output is combined to produce different octaves of noise.
		 */
		std::vector<std::unique_ptr<NoiseGeneratorImproved>> generatorCollection;
		int octaves;

	public:
		/**
		 * Constructs a NoiseGeneratorOctaves with the specified seed and number of octaves
		 *
		 * @param seed The random seed to use
		 * @param octavesIn The number of octaves to generate
		 */
		NoiseGeneratorOctaves(unsigned int seed, int octaves);

		/**
		 * pars:(par2,3,4=noiseOffset ; so that adjacent noise segments connect) (pars5,6,7=x,y,zArraySize),
		 * (pars8,10,12 = x,y,z noiseScale)
		 */
		void GenerateNoiseOctaves(std::vector<double>& noiseArray,
			int xOffset, int yOffset, int zOffset,
			int xSize, int ySize, int zSize,
			double xScale, double yScale, double zScale);

		/**
		 * Bouncer function to the main one with some default arguments.
		 */
		void GenerateNoiseOctaves(std::vector<double>& noiseArray,
			int xOffset, int zOffset,
			int xSize, int zSize,
			double xScale, double zScale);
	};
}