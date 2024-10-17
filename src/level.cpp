#include "level.h"
#include "log.h"
#include "fast_noise_lite.h"

namespace Eon
{
	Level::Level() : sky_color{}
	{
		//allocate all chunks in the level

		for (int x = 0; x < LEVEL_WIDTH_CHUNKS; x++)
		{
			for (int z = 0; z < LEVEL_WIDTH_CHUNKS; z++)
			{
				u32 index = IndexFromPosition(x, z);
				chunks[index] = std::make_unique<Chunk>(ChunkPosition(x, z));
			}
		}

		FastNoiseLite noise;
		noise.SetFractalOctaves(7);
		noise.SetFractalType(FastNoiseLite::FractalType_FBm);

		for (int cx = 0; cx < LEVEL_WIDTH_CHUNKS; cx++)
		{
			for (int cz = 0; cz < LEVEL_WIDTH_CHUNKS; cz++)
			{
				Chunk* chunk = GetChunkUnsafe(ChunkPosition(cx, cz));

				for (int x = 0; x < CHUNK_WIDTH; x++)
				{
					for (int z = 0; z < CHUNK_WIDTH; z++)
					{
						int nx = (cx * CHUNK_WIDTH) + x;
						int nz = (cz * CHUNK_WIDTH) + z;

						int height = (noise.GetNoise(static_cast<float>(nx * 0.1f), static_cast<float>(nz * 0.1f)) + 1) * 128;

						if (cx == 31)
						{
							height -= x * 2;
						}
						if (cz == 31)
						{
							height -= z * 2;
						}
						if (cx == 0)
						{
							height -= (CHUNK_WIDTH - x) * 2;
						}
						if (cz == 0)
						{
							height -= (CHUNK_WIDTH - z) * 2;
						}

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

		if (index >= chunks.size() || index < 0 || chunks[index] == nullptr)
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

	Chunk* Level::GetChunkFromIndex(size_t index)
	{
		if (index >= chunks.size() || index < 0 || chunks[index] == nullptr)
		{
			return nullptr;
		}

		return chunks[index].get();
	}

	Block* Level::GetBlock(i16 x, i16 y, i16 z)
	{
		auto chunk = GetChunk(ChunkPosition{ .x = static_cast<u8>(x >> 5),
											  .z = static_cast<u8>(z >> 5) });

		if (chunk != nullptr)
		{
			u8 bpx = x - (chunk->Position().x * CHUNK_WIDTH);
			u8 bpz = z - (chunk->Position().z * CHUNK_WIDTH);

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
		return (z * LEVEL_WIDTH_CHUNKS) + x;
	}
}  // namespace Eon
