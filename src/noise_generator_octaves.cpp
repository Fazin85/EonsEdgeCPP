#include "noise_generator_octaves.h"
#include <cmath>

namespace Eon {
	NoiseGeneratorOctaves::NoiseGeneratorOctaves(unsigned int seed, int octaves) : octaves(octaves) {
		// Initialize the random generator
		std::mt19937 rng(seed);

		// Resize the collection to hold the specified number of generators
		generatorCollection.resize(octaves);

		// Create each noise generator with a different seed derived from the main seed
		for (int i = 0; i < octaves; ++i) {
			// Create a new seed for each generator
			unsigned int genSeed = rng();
			generatorCollection[i] = std::make_unique<NoiseGeneratorImproved>(genSeed);
		}
	}

	void NoiseGeneratorOctaves::GenerateNoiseOctaves(std::vector<double>& noiseArray,
		int xOffset, int yOffset, int zOffset,
		int xSize, int ySize, int zSize,
		double xScale, double yScale, double zScale) {
		// If no array is provided, create a new one
		if (noiseArray.empty()) {
			noiseArray.resize(xSize * ySize * zSize, 0.0);
		}
		else {
			// Otherwise, clear the existing array
			std::fill(noiseArray.begin(), noiseArray.end(), 0.0);
		}

		double d3 = 1.0;

		// Generate noise for each octave
		for (int j = 0; j < octaves; ++j) {
			double d0 = static_cast<double>(xOffset) * d3 * xScale;
			double d1 = static_cast<double>(yOffset) * d3 * yScale;
			double d2 = static_cast<double>(zOffset) * d3 * zScale;

			// Calculate offsets for consistent noise
			long k = static_cast<long>(std::floor(d0));
			long l = static_cast<long>(std::floor(d2));
			d0 = d0 - static_cast<double>(k);
			d2 = d2 - static_cast<double>(l);

			// Wrap around the noise space
			k = k % 16777216L;
			l = l % 16777216L;
			d0 = d0 + static_cast<double>(k);
			d2 = d2 + static_cast<double>(l);

			// Populate the noise array with this octave's contribution
			generatorCollection[j]->PopulateNoiseArray(noiseArray, d0, d1, d2,
				xSize, ySize, zSize,
				xScale * d3, yScale * d3, zScale * d3, d3);

			// Scale down for the next octave
			d3 /= 2.0;
		}
	}

	void NoiseGeneratorOctaves::GenerateNoiseOctaves(std::vector<double>& noiseArray,
		int xOffset, int zOffset,
		int xSize, int zSize,
		double xScale, double zScale) {
		// Call the main implementation with default values for some parameters
		GenerateNoiseOctaves(noiseArray, xOffset, 10, zOffset, xSize, 1, zSize, xScale, 1.0, zScale);
	}
}