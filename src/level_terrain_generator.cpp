#include "level_terrain_generator.h"

#include "log.h"

void Eon::LevelTerrainGenerator::Init(Level* level)
{
	this->level = level;
}

void Eon::LevelTerrainGenerator::Generate()
{
	if (level == nullptr)
	{
		EON_ERROR("LevelTerrainGenerator: level was nullptr");
		return;
	}

	for (auto& pass : passes)
	{
		pass.Apply(level);
	}
}
