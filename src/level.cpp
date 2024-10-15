#include "level.h"
#include "log.h"
#include "fast_noise_lite.h"

namespace Eon
{
	Level::Level() : sky_color{}
	{
		//allocate all chunks in the level

		for (int x = 0; x < 32; x++)
		{
			for (int z = 0; z < 32; z++)
			{
				u32 index = IndexFromPosition(x, z);
				chunks[index] = std::make_unique<Chunk>(ChunkPosition(x, z));
			}
		}

		FastNoiseLite noise;
		noise.SetFractalOctaves(6);
		noise.SetFractalType(FastNoiseLite::FractalType_FBm);

		for (int cx = 0; cx < 32; cx++)
		{
			for (int cz = 0; cz < 32; cz++)
			{
				Chunk* chunk = GetChunkUnsafe(ChunkPosition(cx, cz));

				for (int x = 0; x < 16; x++)
				{
					for (int z = 0; z < 16; z++)
					{
						int nx = (cx * 16) + x;
						int nz = (cz * 16) + z;

						int height = (noise.GetNoise(static_cast<float>(nx * 0.1f), static_cast<float>(nz * 0.1f)) + 1) * 256;

						for (int y = 0; y < height; y++)
						{
							chunk->GetBlock(x, y, z)->type = BlockType::STONE;
						}
					}
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

		return chunks[index].get();
	}

	Chunk* Level::GetChunkUnsafe(ChunkPosition position)
	{
		u32 index = IndexFromPosition(position.x, position.z);
		return chunks[index].get();
	}

	Block* Level::GetBlock(i16 x, i16 y, i16 z)
	{
		auto chunk = GetChunk(ChunkPosition{ .x = static_cast<u8>(x >> 4),
											  .z = static_cast<u8>(z >> 4) });

		if (chunk != nullptr)
		{
			u8 bpx = x - (chunk->Position().x * 16);
			u8 bpz = z - (chunk->Position().z * 16);

			return chunk->GetBlock(bpx, y, bpz);
		}

		return nullptr;
	}

	Block* Level::GetBlock(glm::ivec3 position)
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
