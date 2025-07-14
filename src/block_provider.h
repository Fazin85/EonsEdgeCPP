#pragma once

namespace Eon
{
	class Block;

	class BlockProvider
	{
	public:
		virtual ~BlockProvider() = default;

		virtual const Block& GetBlock(int x, int y, int z) = 0;
	};
}