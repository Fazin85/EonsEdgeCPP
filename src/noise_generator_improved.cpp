#include "noise_generator_improved.h"

namespace Eon {
	// Define static members
	constexpr double NoiseGeneratorImproved::GRAD_X[];
	constexpr double NoiseGeneratorImproved::GRAD_Y[];
	constexpr double NoiseGeneratorImproved::GRAD_Z[];
	constexpr double NoiseGeneratorImproved::GRAD_2X[];
	constexpr double NoiseGeneratorImproved::GRAD_2Z[];

	NoiseGeneratorImproved::NoiseGeneratorImproved() : NoiseGeneratorImproved(std::random_device{}()) {
	}

	NoiseGeneratorImproved::NoiseGeneratorImproved(unsigned int seed) {
		std::mt19937 rng(seed);
		std::uniform_real_distribution<double> dist(0.0, 256.0);

		permutations.resize(512);
		xCoord = dist(rng);
		yCoord = dist(rng);
		zCoord = dist(rng);

		// Initialize permutations with values 0-255
		for (int i = 0; i < 256; i++) {
			permutations[i] = i;
		}

		// Shuffle permutations
		for (int l = 0; l < 256; ++l) {
			std::uniform_int_distribution<int> intDist(l, 255);
			int j = intDist(rng);
			int k = permutations[l];
			permutations[l] = permutations[j];
			permutations[j] = k;
			permutations[l + 256] = permutations[l];
		}
	}

	double NoiseGeneratorImproved::Lerp(double t, double a, double b) const {
		return a + t * (b - a);
	}

	double NoiseGeneratorImproved::Grad2(int hash, double x, double z) const {
		int idx = hash & 15;
		return GRAD_2X[idx] * x + GRAD_2Z[idx] * z;
	}

	double NoiseGeneratorImproved::Grad(int hash, double x, double y, double z) const {
		int idx = hash & 15;
		return GRAD_X[idx] * x + GRAD_Y[idx] * y + GRAD_Z[idx] * z;
	}

	void NoiseGeneratorImproved::PopulateNoiseArray(std::vector<double>& noiseArray,
		double xOffset, double yOffset, double zOffset,
		int xSize, int ySize, int zSize,
		double xScale, double yScale, double zScale,
		double noiseScale) {
		if (ySize == 1) {
			int i5 = 0;
			int j5 = 0;
			int j = 0;
			int k5 = 0;
			double d14 = 0.0;
			double d15 = 0.0;
			int l5 = 0;
			double d16 = 1.0 / noiseScale;

			for (int j2 = 0; j2 < xSize; ++j2) {
				double d17 = xOffset + static_cast<double>(j2) * xScale + xCoord;
				int i6 = static_cast<int>(d17);

				if (d17 < static_cast<double>(i6)) {
					--i6;
				}

				int k2 = i6 & 255;
				d17 = d17 - static_cast<double>(i6);
				double d18 = d17 * d17 * d17 * (d17 * (d17 * 6.0 - 15.0) + 10.0);

				for (int j6 = 0; j6 < zSize; ++j6) {
					double d19 = zOffset + static_cast<double>(j6) * zScale + zCoord;
					int k6 = static_cast<int>(d19);

					if (d19 < static_cast<double>(k6)) {
						--k6;
					}

					int l6 = k6 & 255;
					d19 = d19 - static_cast<double>(k6);
					double d20 = d19 * d19 * d19 * (d19 * (d19 * 6.0 - 15.0) + 10.0);
					i5 = permutations[k2] + 0;
					j5 = permutations[i5] + l6;
					j = permutations[k2 + 1] + 0;
					k5 = permutations[j] + l6;
					d14 = Lerp(d18, Grad2(permutations[j5], d17, d19), Grad(permutations[k5], d17 - 1.0, 0.0, d19));
					d15 = Lerp(d18, Grad(permutations[j5 + 1], d17, 0.0, d19 - 1.0), Grad(permutations[k5 + 1], d17 - 1.0, 0.0, d19 - 1.0));
					double d21 = Lerp(d20, d14, d15);
					int i7 = l5++;
					noiseArray[i7] += d21 * d16;
				}
			}
		}
		else {
			int i = 0;
			double d0 = 1.0 / noiseScale;
			int k = -1;
			int l = 0;
			int i1 = 0;
			int j1 = 0;
			int k1 = 0;
			int l1 = 0;
			int i2 = 0;
			double d1 = 0.0;
			double d2 = 0.0;
			double d3 = 0.0;
			double d4 = 0.0;

			for (int l2 = 0; l2 < xSize; ++l2) {
				double d5 = xOffset + static_cast<double>(l2) * xScale + xCoord;
				int i3 = static_cast<int>(d5);

				if (d5 < static_cast<double>(i3)) {
					--i3;
				}

				int j3 = i3 & 255;
				d5 = d5 - static_cast<double>(i3);
				double d6 = d5 * d5 * d5 * (d5 * (d5 * 6.0 - 15.0) + 10.0);

				for (int k3 = 0; k3 < zSize; ++k3) {
					double d7 = zOffset + static_cast<double>(k3) * zScale + zCoord;
					int l3 = static_cast<int>(d7);

					if (d7 < static_cast<double>(l3)) {
						--l3;
					}

					int i4 = l3 & 255;
					d7 = d7 - static_cast<double>(l3);
					double d8 = d7 * d7 * d7 * (d7 * (d7 * 6.0 - 15.0) + 10.0);

					for (int j4 = 0; j4 < ySize; ++j4) {
						double d9 = yOffset + static_cast<double>(j4) * yScale + yCoord;
						int k4 = static_cast<int>(d9);

						if (d9 < static_cast<double>(k4)) {
							--k4;
						}

						int l4 = k4 & 255;
						d9 = d9 - static_cast<double>(k4);
						double d10 = d9 * d9 * d9 * (d9 * (d9 * 6.0 - 15.0) + 10.0);

						if (j4 == 0 || l4 != k) {
							k = l4;
							l = permutations[j3] + l4;
							i1 = permutations[l] + i4;
							j1 = permutations[l + 1] + i4;
							k1 = permutations[j3 + 1] + l4;
							l1 = permutations[k1] + i4;
							i2 = permutations[k1 + 1] + i4;
							d1 = Lerp(d6, Grad(permutations[i1], d5, d9, d7), Grad(permutations[l1], d5 - 1.0, d9, d7));
							d2 = Lerp(d6, Grad(permutations[j1], d5, d9 - 1.0, d7), Grad(permutations[i2], d5 - 1.0, d9 - 1.0, d7));
							d3 = Lerp(d6, Grad(permutations[i1 + 1], d5, d9, d7 - 1.0), Grad(permutations[l1 + 1], d5 - 1.0, d9, d7 - 1.0));
							d4 = Lerp(d6, Grad(permutations[j1 + 1], d5, d9 - 1.0, d7 - 1.0), Grad(permutations[i2 + 1], d5 - 1.0, d9 - 1.0, d7 - 1.0));
						}

						double d11 = Lerp(d10, d1, d2);
						double d12 = Lerp(d10, d3, d4);
						double d13 = Lerp(d8, d11, d12);
						int j7 = i++;
						noiseArray[j7] += d13 * d0;
					}
				}
			}
		}
	}
}