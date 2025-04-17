#include "chunk.h"
#include "log.h"

#define INDEX_FROM_POSITION(x, y, z) x + CHUNK_WIDTH *(y + CHUNK_HEIGHT * z)

namespace Eon
{
	Chunk::Chunk(ChunkPrimer& chunkPrimer, ChunkPosition chunkPosition) :
		position(chunkPosition),
		decorated(false),
		sections(),
		column_heights(),
		mutex(),
		can_unload(true),
		aabb(glm::vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)),
		chunk_heights(0, CHUNK_HEIGHT - 1)
	{
		aabb.Update(glm::vec3(position.x, 0, position.z));

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

		std::lock_guard<std::mutex> lock(mutex);

		for (int x = 0; x < CHUNK_WIDTH; x++) {
			for (int z = 0; z < CHUNK_WIDTH; z++) {
				CalculateColumnHeights(x, z, false);
			}
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

		CalculateColumnHeights(x, z, false);
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

	Chunk::ColumnHeights Chunk::GetColumnHeights(int x, int z)
	{
		if (x >= CHUNK_WIDTH || x < 0 || z >= CHUNK_WIDTH || z < 0) {
			return Chunk::ColumnHeights{ CHUNK_HEIGHT - 1, 0 };
		}

		return column_heights[x][z];
	}

	Chunk::ColumnHeights Chunk::GetChunkHeights() const
	{
		return chunk_heights;
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

	bool Chunk::IsSectionEmpty(int sectionIndex)
	{
		if (sectionIndex >= sections.size() || sectionIndex < 0) {
			return false;
		}

		Block outBlock;
		return sections[sectionIndex].IsUniform(outBlock);
	}

	void Chunk::SetCanUnload(bool canUnload)
	{
		can_unload = canUnload;
	}

	AABB& Chunk::GetAABB()
	{
		return aabb;
	}

	Block Chunk::GetBlockInternal(int x, int y, int z, bool lock)
	{
		if (lock) {
			mutex.lock();
		}

		int sy = y >> 4;
		Block block = sections[sy].GetBlock(x, std::abs(y - (sy * 16)), z);

		if (lock) {
			mutex.unlock();
		}

		return block;
	}

	void Chunk::CalculateColumnHeights(int x, int z, bool lock)
	{
		if (lock) {
			mutex.lock();
		}

		for (int y = CHUNK_HEIGHT - 1; y >= 0; y--) {
			if (GetBlockInternal(x, y, z, false).type != BlockType::AIR) {
				column_heights[x][z].highest = y;
				break;
			}
		}

		for (int y = 0; y < CHUNK_HEIGHT - 1; y++) {
			Block currentBlock = GetBlockInternal(x, y, z, false);
			Block aboveBlock = GetBlockInternal(x, y + 1, z, false);

			if ((!currentBlock.IsCutout() && !currentBlock.Translucent()) && (aboveBlock.IsCutout() || aboveBlock.Translucent())) {
				column_heights[x][z].lowest = y;
				if (y < chunk_heights.lowest) {
					chunk_heights.lowest = y;
				}
				break;
			}
		}

		if (lock) {
			mutex.unlock();
		}
	}
} // namespace Eon
