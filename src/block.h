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
		SAND,
		OAKLOG,
		LEAF,
		GRAVEL
	};

	struct Block
	{
		Block()
		{
			type = BlockType::AIR;
			//light = 15;
		}

		Block(BlockType t) : type(t) {}

		BlockType type;
		//unsigned char light;
	};
}  // namespace Eon