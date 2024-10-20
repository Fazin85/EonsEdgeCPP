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
		Block()
		{
			type = BlockType::AIR;
			//light = 15;
		}

		BlockType type;
		//u8 light;
	};
}  // namespace Eon