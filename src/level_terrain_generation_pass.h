#pragma once

#include <array>
#include "level.h"

namespace Eon
{
	class LevelTerrainGenerationPass
	{
	public:
		virtual void Apply(Level* level) = 0;
	};
}