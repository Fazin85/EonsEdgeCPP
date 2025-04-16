#include "chunk.h"
#include "log.h"

#define INDEX_FROM_POSITION(x, y, z) x + CHUNK_WIDTH *(y + CHUNK_HEIGHT * z)

namespace Eon
{
	Chunk::Chunk(ChunkPrimer& chunkPrimer, ChunkPosition chunkPosition) :
		position(chunkPosition),
		decorated(false),
		sections(),
		highest_blocks(std::make_unique < std::array<short, CHUNK_WIDTH* CHUNK_WIDTH>>()),
		mutex(),
		can_unload(true)
	{
		for (int x = 0; x < CHUNK_WIDTH; x++) {
			for (int y = 0; y < CHUNK_HEIGHT; y++) {
				for (int z = 0; z < CHUNK_WIDTH; z++) {
					int sy = y >> 4;
					sections[sy].SetBlock(x, std::abs(y - (sy * 16)), z, chunkPrimer.GetBlock(x, y, z));
				}
			}
		}

		for (ChunkSection& section : sections) {
			section.CalculateUniformity();
		}
	}

	void Chunk::SetBlock(int x, int y, int z, Block& block) {
		std::lock_guard<std::mutex> lock(mutex);

		if (x >= CHUNK_WIDTH || z >= CHUNK_WIDTH || y >= CHUNK_HEIGHT || x < 0 ||
			z < 0 || y < 0)
		{
			return;
		}

		int sy = y >> 4;
		sections[sy].SetBlock(x, std::abs(y - (sy * 16)), z, block);
		sections[sy].CalculateUniformity();
	}

	Block Chunk::GetBlock(int x, int y, int z)
	{
		std::lock_guard<std::mutex> lock(mutex);

		if (x >= CHUNK_WIDTH || z >= CHUNK_WIDTH || y >= CHUNK_HEIGHT || x < 0 ||
			z < 0 || y < 0)
		{
			return { BlockType::AIR };
		}

		int sy = y >> 4;
		return sections[sy].GetBlock(x, std::abs(y - (sy * 16)), z);
	}

	ChunkPosition Chunk::Position() const { return position; }

	short Chunk::GetHeightestBlockY(int x, int z)
	{
		return highest_blocks->data()[(x * CHUNK_WIDTH) + z];
	}

	void Chunk::SetDecorated(bool decorated)
	{
		this->decorated = decorated;
	}

	bool Chunk::IsDecorated() const
	{
		return decorated;
	}

	bool Chunk::CanUnload() const
	{
		return can_unload;
	}

	void Chunk::SetCanUnload(bool canUnload)
	{
		can_unload = canUnload;
	}
} // namespace Eon
