#include "fast_noise_lite.h"
#include "level.h"
#include "log.h"
#include "scope_timer.h"
#include <filesystem>
#include <future>
#include <random>
#include <thread>

namespace Eon
{
	Level::Level() : chunks_files{}, sky_color{}
	{
		//bool write = !std::filesystem::exists("level.dat");
		bool write = true;
		std::filesystem::path directoryPath = "level";

		if (!std::filesystem::exists(directoryPath))
		{
			if (std::filesystem::create_directory(directoryPath))
			{
				EON_INFO("Directory created successfully: level");
			}
			else
			{
				EON_ERROR("Error creating directory: level");
			}
		}
		else
		{
			EON_INFO("Directory already exists: level");
		}

		tree_model = std::make_unique<VoxelModel>("tree.vox");

		//save_file.open("level.dat", std::ios::binary);

		//allocate all chunks in the level

		for (int x = 0; x < LEVEL_WIDTH_CHUNKS; x++)
		{
			for (int z = 0; z < LEVEL_WIDTH_CHUNKS; z++)
			{
				unsigned int index = IndexFromPosition(x, z);
				chunks[index] = std::make_unique<Chunk>(ChunkPosition(x, z));
			}
		}

		FastNoiseLite noise;
		noise.SetFractalOctaves(10);
		noise.SetFractalType(FastNoiseLite::FractalType_FBm);
		std::mt19937 rng(0);

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

						int height = (noise.GetNoise(static_cast<float>(nx * 0.15f), static_cast<float>(nz * 0.15f)) + 1) * 48;

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
								*chunk->GetHeightestBlockY(x, z) = y;
							}
							else
							{
								chunk->GetBlock(x, y, z)->type = BlockType::DIRT;
							}
						}

						std::uniform_int_distribution<int> gen(0, 100);
						if (gen(rng) == 0)
						{
							PlaceModel(*tree_model, nx, *chunk->GetHeightestBlockY(x, z), nz);
							//PlaceTree(nx, nz);
						}
					}
				}

				//chunk->Compress();
			}
		}

		//set the max file handles
		if (_setmaxstdio(2048) == -1)
		{
			EON_CRITICAL("_setmaxstdio failed");
			std::abort();
		}

		if (write)
		{
			ScopeTimer timer("Write");
			std::vector<std::future<void>> fileFutures;

			for (int x = 0; x < LEVEL_WIDTH_CHUNKS; x++)
			{
				for (int z = 0; z < LEVEL_WIDTH_CHUNKS; z++)
				{
					ChunkPosition position(x, z);

					fileFutures.push_back(std::async(std::launch::async, [this, position]
						{
							auto index = IndexFromPosition(position.x, position.z);
							chunks_files[index] = std::make_unique<std::fstream>("level/" + std::to_string(position.x) + "." + std::to_string(position.z) + ".dat", std::ios::out | std::ios::binary);
							std::fstream& chunkFile = *chunks_files[index];
							Chunk* chunk = GetChunk(position);

							if (chunk != nullptr)
							{
								auto data = chunk->CompressToBuffer();
								size_t size = data.size();

								chunkFile.write(reinterpret_cast<char*>(&size), sizeof(size_t));
								chunkFile.write(data.data(), data.size());
							}
						}));
				}
			}

			for (auto& future : fileFutures)
			{
				future.wait();
			}
		}

		//save_file.close();
	}

	Level::~Level()
	{
		for (auto& file : chunks_files)
		{
			if (file == nullptr)
			{
				EON_ERROR("Chunk file was nullptr");
			}
			else
			{
				file->close();
			}
		}
	}

	Chunk* Level::GetChunk(ChunkPosition position)
	{
		unsigned int index = IndexFromPosition(position.x, position.z);

		if (index >= chunks.size() || index < 0 || chunks[index] == nullptr)
		{
			return nullptr;
		}

		return chunks[index].get();
	}

	Chunk* Level::GetChunkUnsafe(ChunkPosition position)
	{
		unsigned int index = IndexFromPosition(position.x, position.z);
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

	Block* Level::GetBlock(short x, short y, short z)
	{
		auto chunk = GetChunk(ChunkPosition{ .x = static_cast<unsigned char>(x >> CHUNK_BITSHIFT_AMOUNT),
											  .z = static_cast<unsigned char>(z >> CHUNK_BITSHIFT_AMOUNT) });

		if (chunk != nullptr)
		{
			unsigned char bpx = x - (chunk->Position().x * CHUNK_WIDTH);
			unsigned char bpz = z - (chunk->Position().z * CHUNK_WIDTH);

			return chunk->GetBlock(bpx, y, bpz);
		}

		return nullptr;
	}

	void Level::SetBlock(Block block, short x, short y, short z)
	{
		Block* blockPtr = GetBlock(x, y, z);

		if (blockPtr != nullptr)
		{
			*blockPtr = block;
		}
	}

	Block* Level::GetBlock(glm::ivec3 position)
	{
		return GetBlock(static_cast<short>(position.x), static_cast<short>(position.y), static_cast<short>(position.z));
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

	unsigned int Level::IndexFromPosition(short x, short z)
	{
		return (x * LEVEL_WIDTH_CHUNKS) + z;
	}

	short Level::GetHighestBlockY(short x, short z)
	{
		Chunk* chunk = GetChunk(ChunkPosition{ .x = static_cast<unsigned char>(x >> CHUNK_BITSHIFT_AMOUNT),
											  .z = static_cast<unsigned char>(z >> CHUNK_BITSHIFT_AMOUNT) });

		if (chunk == nullptr)
		{
			return 0;
		}

		return *chunk->GetHeightestBlockY(x - (chunk->Position().x * CHUNK_WIDTH), z - (chunk->Position().z * CHUNK_WIDTH));
	}

	Block* Level::GetBlockDecompressChunk(glm::ivec3 position)
	{
		auto chunk = GetChunk(ChunkPosition{ .x = static_cast<unsigned char>(position.x >> CHUNK_BITSHIFT_AMOUNT),
											  .z = static_cast<unsigned char>(position.z >> CHUNK_BITSHIFT_AMOUNT) });
		if (chunk != nullptr)
		{
			chunk->Decompress();

			unsigned char bpx = position.x - (chunk->Position().x * CHUNK_WIDTH);
			unsigned char bpz = position.z - (chunk->Position().z * CHUNK_WIDTH);

			return chunk->GetBlock(bpx, position.y, bpz);
		}

		return nullptr;
	}

	void Level::PlaceModel(VoxelModel& model, short x, short y, short z)
	{
		for (int mx = 0; mx < model.Size().x; mx++)
		{
			for (int my = 0; my < model.Size().y; my++)
			{
				for (int mz = 0; mz < model.Size().z; mz++)
				{
					BlockType type = model.GetBlockType(mx, my, mz);

					if (type != BlockType::AIR)
					{
						SetBlock(Block(type), x + mx, y + my, z + mz);
					}
				}
			}
		}
	}

	void Level::LoadChunkDataFromFilesystem(Chunk& chunk)
	{

	}

	void Level::PlaceTree(short x, short z)
	{
		Chunk* chunk = GetChunk(ChunkPosition{ .x = static_cast<unsigned char>(x >> CHUNK_BITSHIFT_AMOUNT),
											  .z = static_cast<unsigned char>(z >> CHUNK_BITSHIFT_AMOUNT) });

		if (chunk == nullptr)
		{
			return;
		}

		short y = *chunk->GetHeightestBlockY(x - (chunk->Position().x * CHUNK_WIDTH), z - (chunk->Position().z * CHUNK_WIDTH));

		for (int yp = y; yp < y + 6; yp++)
		{
			GetBlock(x, yp, z)->type = BlockType::OAKLOG;
		}

		for (int px = x - 3; px < x + 3; px++)
		{
			for (int pz = z - 3; pz < z + 3; pz++)
			{
				SetBlock(Block(Eon::BlockType::LEAF), px, y + 5, pz);
			}
		}
	}
}  // namespace Eon
