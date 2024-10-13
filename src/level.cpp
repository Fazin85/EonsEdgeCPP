#include "level.h"
#include "log.h"

namespace Eon
{
	Level::Level() : chunks{}
	{
		chunks[0] = new Chunk(ChunkPosition{ .x = 0, .z = 0 });

		for (int x = 0; x < 16; x++)
		{
			for (int z = 0; z < 16; z++)
			{
				for (int y = 0; y < 32; y++)
				{
					chunks[0]->GetBlock(x, y, z).value()->type = BlockType::STONE;
				}
			}
		}
	}

	Chunk* Level::GetChunk(ChunkPosition position)
	{
		u32 index = IndexFromPosition(position.x, position.z);

		if (index >= 1024 || index < 0 || chunks[index] == nullptr)
		{
			return nullptr;
		}

		return chunks[index];
	}

	std::optional<Block> Level::GetBlock(i16 x, i16 y, i16 z)
	{
		auto chunk = GetChunk(ChunkPosition{ .x = static_cast<u8>(x >> 4),
											  .z = static_cast<u8>(z >> 4) });

		if (chunk != nullptr)
		{
			u8 bpx = x - chunk->Position().x;
			u8 bpz = x - chunk->Position().z;

			auto block = chunk->GetBlock(bpx, y, bpz);
			if (block.has_value())
			{
				return *block.value();
			}
		}

		return {};
	}

	std::optional<Block> Level::GetBlock(glm::ivec3 position)
	{
		return GetBlock(static_cast<i16>(position.x), static_cast<i16>(position.y), static_cast<i16>(position.z));
	}

	glm::vec4& Level::SkyColor()
	{
		return sky_color;
	}

	u32 Level::IndexFromPosition(i16 x, i16 z)
	{
		return (z * 32) + x;
	}
}  // namespace Eon
