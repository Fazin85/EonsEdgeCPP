#pragma once

#include "num.h"

namespace Eon
{
	enum class BlockType : u8
	{
		AIR,
		STONE,
		GRASS,
		DIRT,
		WATER,
		SAND
	};

	struct Block
	{
		BlockType type;
		u8 light;
	};
}  // namespace Eon