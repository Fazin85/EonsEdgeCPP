#pragma once



namespace Eon
{
	enum class BlockType : unsigned char
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
		//unsigned char light;
	};
}  // namespace Eon