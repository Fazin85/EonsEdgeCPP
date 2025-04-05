#include "chunk.h"
#include "log.h"

#define INDEX_FROM_POSITION(x, y, z) x + CHUNK_WIDTH *(y + CHUNK_HEIGHT * z)

namespace Eon
{
	Chunk::Chunk(ChunkPosition chunkPosition) :
		position(chunkPosition),
		decorated(false),
		blocks(std::make_unique < std::array<Block, CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_WIDTH>>()),
		highest_blocks(std::make_unique < std::array<short, CHUNK_WIDTH* CHUNK_WIDTH>>()),
		mutex()
	{
	}

	void Chunk::SetBlock(int x, int y, int z, Block& block) {
		std::lock_guard<std::mutex> lock(mutex);

		if (x >= CHUNK_WIDTH || z >= CHUNK_WIDTH || y >= CHUNK_HEIGHT || x < 0 ||
			z < 0 || y < 0)
		{
			return;
		}

		blocks->data()[INDEX_FROM_POSITION(x, y, z)] = block;
	}

	Block Chunk::GetBlock(int x, int y, int z)
	{
		std::lock_guard<std::mutex> lock(mutex);

		if (x >= CHUNK_WIDTH || z >= CHUNK_WIDTH || y >= CHUNK_HEIGHT || x < 0 ||
			z < 0 || y < 0)
		{
			return { BlockType::AIR };
		}

		return blocks->data()[INDEX_FROM_POSITION(x, y, z)];
	}

	ChunkPosition Chunk::Position() const { return position; }

	std::array<Block, CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_WIDTH>& Chunk::GetBlocks() {
		std::lock_guard<std::mutex> lock(mutex);

		return *blocks;
	}

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
} // namespace Eon
