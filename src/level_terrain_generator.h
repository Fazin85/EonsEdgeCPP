#pragma once

#include <vector>
#include "level_terrain_generation_pass.h"

namespace Eon
{
	class LevelTerrainGenerator
	{
		void Init(Level* level);
		void Generate();

	private:
		std::vector<LevelTerrainGenerationPass> passes;
		Level* level;
	};
}