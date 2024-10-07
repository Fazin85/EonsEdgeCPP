#include "level.h"

namespace Eon
{
	Chunk* Level::GetChunk(ChunkPosition position)
	{
		// TODO: insert return statement here
		u32 index = IndexFromPosition(position.x, position.z);
		if (index >= 1024 || index < 0)
		{
			return nullptr;
		}

		return chunks[index];
	}

	Block* Level::GetBlock(u16 x, u16 y, u16 z)
	{
		Chunk* chunk = GetChunk(ChunkPosition{ .x = static_cast<u8>(x >> 4),
											  .z = static_cast<u8>(z >> 4) });

		u8 bpx = x - chunk->Position().x;
		u8 bpz = x - chunk->Position().z;
		return chunk->GetBlock(bpx, y, bpz);
	}

	Block* Level::GetBlock(glm::ivec3 position)
	{
		return GetBlock(static_cast<u16>(position.x), static_cast<u16>(position.y), static_cast<u16>(position.z));
	}

	u32 Level::IndexFromPosition(u16 x, u16 z)
	{
		return (x * 32) + z;
	}
}  // namespace Eon
