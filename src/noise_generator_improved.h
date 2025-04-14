#pragma once

#include <random>
#include <vector>

namespace Eon {
	class NoiseGeneratorImproved {
	private:
		/**
		 * An int[512], where the first 256 elements are the numbers 0..255, in random shuffled order,
		 * and the second half of the array is identical to the first half, for convenience in wrapping lookups.
		 *
		 * Effectively a shuffled 0..255 that wraps once.
		 */
		std::vector<int> permutations;

		static constexpr double GRAD_X[] = { 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 };
		static constexpr double GRAD_Y[] = { 1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0 };
		static constexpr double GRAD_Z[] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0 };
		static constexpr double GRAD_2X[] = { 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 };
		static constexpr double GRAD_2Z[] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0 };

	public:
		double xCoord;
		double yCoord;
		double zCoord;

		// Default constructor using random seed
		NoiseGeneratorImproved();

		// Constructor with specific seed
		explicit NoiseGeneratorImproved(unsigned int seed);

		// Linear interpolation between two values
		double Lerp(double t, double a, double b) const;

		// Gradient function for 2D noise
		double Grad2(int hash, double x, double z) const;

		// Gradient function for 3D noise
		double Grad(int hash, double x, double y, double z) const;

		/**
		 * Populates a noise array with Perlin noise values
		 * noiseArray should be xSize*ySize*zSize in size
		 */
		void PopulateNoiseArray(std::vector<double>& noiseArray,
			double xOffset, double yOffset, double zOffset,
			int xSize, int ySize, int zSize,
			double xScale, double yScale, double zScale,
			double noiseScale);
	};
}