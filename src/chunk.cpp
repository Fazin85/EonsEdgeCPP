#include "level.h"
#include "chunk.h"
#include "log.h"

#define INDEX_FROM_POSITION(x, y, z) x + CHUNK_WIDTH *(y + CHUNK_HEIGHT * z)

namespace Eon
{
	Chunk::Chunk(Level& level, const ChunkPrimer& chunkPrimer, ChunkPosition chunkPosition) :
		level(level),
		position(chunkPosition),
		decorated(false),
		sections(),
		column_heights(),
		mutex(),
		can_unload(true),
		aabb(glm::vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)),
		chunk_heights{ 0, CHUNK_HEIGHT - 1 },
		block_entities()
	{
		aabb.Update(glm::vec3(position.x, 0, position.z));

		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				for (int z = 0; z < CHUNK_WIDTH; z++)
				{
					int sy = y >> 4;
					sections[sy].SetBlock(x, std::abs(y - (sy * 16)), z, chunkPrimer.GetBlock(x, y, z));
				}
			}
		}

		for (ChunkSection& section : sections)
		{
			section.CalculateUniformity();
		}

		std::scoped_lock<std::mutex> lock(mutex);

		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int z = 0; z < CHUNK_WIDTH; z++)
			{
				CalculateColumnHeights(x, z);
			}
		}
	}

	void Chunk::SetBlock(int x, int y, int z, const Block& b)
	{
		std::scoped_lock<std::mutex> lock(mutex);

		if (x >= CHUNK_WIDTH || z >= CHUNK_WIDTH || y >= CHUNK_HEIGHT || x < 0 ||
			z < 0 || y < 0)
		{
			return;
		}

		int sy = y >> 4;
		sections[sy].SetBlock(x, std::abs(y - (sy * 16)), z, b);
		sections[sy].CalculateUniformity();

		CalculateColumnHeights(x, z);
	}

	const Block& Chunk::GetBlock(int x, int y, int z)
	{
		std::scoped_lock<std::mutex> lock(mutex);

		if (x >= CHUNK_WIDTH || z >= CHUNK_WIDTH || y >= CHUNK_HEIGHT || x < 0 ||
			z < 0 || y < 0)
		{
			if (y < 0)
			{
				return BlockRegistry::GetBlockByID(1);
			}
			else if (y > CHUNK_HEIGHT - 1)
			{
				return BlockRegistry::GetBlockByID(0);
			}

			return level.GetBlock(glm::ivec3(x + position.x, y, z + position.z));
		}

		int sy = y >> 4;
		return sections[sy].GetBlock(x, std::abs(y - (sy * 16)), z);
	}

	ChunkPosition Chunk::Position() const { return position; }

	Chunk::ColumnHeights Chunk::GetColumnHeights(int x, int z)
	{
		if (x >= CHUNK_WIDTH || x < 0 || z >= CHUNK_WIDTH || z < 0)
		{
			return Chunk::ColumnHeights{ CHUNK_HEIGHT - 1, 0 };
		}

		return column_heights[x][z];
	}

	Chunk::ColumnHeights Chunk::GetChunkHeights() const
	{
		return chunk_heights;
	}

	void Chunk::SetDecorated(bool d)
	{
		this->decorated = d;
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
		if (sectionIndex >= sections.size() || sectionIndex < 0)
		{
			return false;
		}

		return sections[sectionIndex].IsUniform().has_value();
	}

	void Chunk::SetCanUnload(bool canUnload)
	{
		can_unload = canUnload;
	}

	AABB& Chunk::GetAABB()
	{
		return aabb;
	}

	std::vector<BlockEntity*> Chunk::GetBlockEntitys() const
	{
		std::vector<BlockEntity*> blockEntitys;

		for (const auto& [pos, blockEntity] : block_entities)
		{
			blockEntitys.emplace_back(blockEntity.get());
		}

		return blockEntitys;
	}

	void Chunk::AddBlockEntity(std::unique_ptr<BlockEntity> blockEntity)
	{
		block_entities[blockEntity->GetWorldPosition()] = std::move(blockEntity);
	}

	const Block& Chunk::GetBlockInternal(int x, int y, int z)
	{
		int sy = y >> 4;
		return sections[sy].GetBlock(x, std::abs(y - (sy * 16)), z);
	}

	void Chunk::CalculateColumnHeights(int x, int z)
	{
		for (int y = CHUNK_HEIGHT - 1; y >= 0; y--)
		{
			if (GetBlockInternal(x, y, z) != BlockRegistry::GetBlockByID(0))
			{
				column_heights[x][z].highest = y;
				break;
			}
		}

		for (int y = 0; y < CHUNK_HEIGHT - 1; y++)
		{
			Block currentBlock = GetBlockInternal(x, y, z);
			Block aboveBlock = GetBlockInternal(x, y + 1, z);

			if ((!currentBlock.IsCutout() && !currentBlock.Translucent()) && (aboveBlock.IsCutout() || aboveBlock.Translucent()))
			{
				column_heights[x][z].lowest = y;
				if (y < chunk_heights.lowest)
				{
					chunk_heights.lowest = y;
				}
				break;
			}
		}
	}
} // namespace Eon
