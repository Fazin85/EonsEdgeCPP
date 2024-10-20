#include "level.h"
#include "log.h"
#include "fast_noise_lite.h"
#include <filesystem>

namespace Eon
{
	Level::Level() : sky_color{}
	{
		//bool write = !std::filesystem::exists("level.dat");
		bool write = true;

		save_file.open("level.dat", std::ios::binary);

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

						int height = (noise.GetNoise(static_cast<float>(nx * 0.1f), static_cast<float>(nz * 0.1f)) + 1) * 64;

						if (height >= CHUNK_HEIGHT)
						{
							continue;
						}

						if (cx == LEVEL_WIDTH_CHUNKS - 1)
						{
							height -= x * 2;
						}
						if (cz == LEVEL_WIDTH_CHUNKS - 1)
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
							if (y == height - 1)
							{
								chunk->GetBlock(x, y, z)->type = BlockType::GRASS;
							}
							else
							{
								chunk->GetBlock(x, y, z)->type = BlockType::STONE;
							}
						}
					}
				}

				//chunk->Compress();
			}
		}

		if (write)
		{
			//save_file.seekp(0, std::ios::beg);
			//constexpr size_t bufferSize = CHUNK_BLOCK_COUNT * sizeof(Block) * (LEVEL_WIDTH_CHUNKS * LEVEL_WIDTH_CHUNKS);
			//char* buffer = new char[bufferSize];
			//std::fill_n(buffer, bufferSize, 0);

			//save_file.write(buffer, bufferSize);

			//delete[] buffer;

			//save_file.seekp(0, std::ios::beg);

			//for (int i = 0; i < LEVEL_WIDTH_CHUNKS * LEVEL_WIDTH_CHUNKS; i++)
			//{
			//	std::vector<char> compressedBlocks = chunks[i]->CompressToBuffer();
			//	save_file.write(reinterpret_cast<char*>(chunks[i]->GetBlocks()), sizeof(Block) * CHUNK_BLOCK_COUNT);
			//}
		}

		save_file.close();
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
		auto chunk = GetChunk(ChunkPosition{ .x = static_cast<u8>(x >> CHUNK_BITSHIFT_AMOUNT),
											  .z = static_cast<u8>(z >> CHUNK_BITSHIFT_AMOUNT) });

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

	std::vector<BoundingBox> Level::GetBlockAABBs(const BoundingBox& bb)
	{
		std::vector<BoundingBox> bbs;

		glm::ivec3 min = glm::ivec3(static_cast<int>(bb.min.x), static_cast<int>(bb.min.y), static_cast<int>(bb.min.z));
		glm::ivec3 max = glm::ivec3(static_cast<int>(bb.max.x) + 1, static_cast<int>(bb.max.y) + 1, static_cast<int>(bb.max.z) + 1);

		for (int x = min.x; x <= max.x; x++)
		{
			for (int y = min.y; y <= max.y; y++)
			{
				for (int z = min.z; z <= max.z; z++)
				{
					Block* block = GetBlockDecompressChunk(glm::ivec3(x, y, z));

					if (block != nullptr)
					{
						if (block->type != BlockType::AIR)
						{
							bbs.emplace_back(glm::vec3(x, y, z), glm::vec3(x + 1, y + 1, z + 1));
						}
					}
				}
			}
		}

		return bbs;
	}

	u32 Level::IndexFromPosition(i16 x, i16 z)
	{
		return (z * LEVEL_WIDTH_CHUNKS) + x;
	}

	Block* Level::GetBlockDecompressChunk(glm::ivec3 position)
	{
		auto chunk = GetChunk(ChunkPosition{ .x = static_cast<u8>(position.x >> CHUNK_BITSHIFT_AMOUNT),
											  .z = static_cast<u8>(position.z >> CHUNK_BITSHIFT_AMOUNT) });
		if (chunk != nullptr)
		{
			chunk->Decompress();

			u8 bpx = position.x - (chunk->Position().x * CHUNK_WIDTH);
			u8 bpz = position.z - (chunk->Position().z * CHUNK_WIDTH);

			return chunk->GetBlock(bpx, position.y, bpz);
		}

		return nullptr;
	}

	void Level::LoadChunkDataFromFilesystem(Chunk& chunk)
	{

	}
}  // namespace Eon
