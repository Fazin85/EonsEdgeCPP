#pragma once

#include "block.h"
#include "chunk_position.h"
#include "chunk_info.h"
#include "chunk_primer.h"
#include "chunk_section.h"
#include "aabb.h"
#include "block_entity.h"

#include <array>
#include <memory>
#include <mutex>

namespace Eon
{
	class Chunk
	{
	public:
		struct ColumnHeights {
			int highest;
			int lowest;
		};

		Chunk(const ChunkPrimer& chunkPrimer, ChunkPosition chunkPosition);
		void SetBlock(int x, int y, int z, const Block& block);
		Block GetBlock(int x, int y, int z);
		ChunkPosition Position() const;
		ColumnHeights GetColumnHeights(int x, int z);
		ColumnHeights GetChunkHeights() const;
		void SetDecorated(bool decorated);
		bool IsDecorated() const;
		bool CanUnload() const;
		bool IsSectionEmpty(int sectionIndex);
		void SetCanUnload(bool canUnload);
		AABB& GetAABB();

	private:
		Block GetBlockInternal(int x, int y, int z);
		void CalculateColumnHeights(int x, int z);

		ChunkPosition position;
		std::array<ChunkSection, CHUNK_HEIGHT / 16> sections;
		std::array<std::array<ColumnHeights, CHUNK_WIDTH>, CHUNK_WIDTH> column_heights;
		std::unordered_map<glm::ivec3, std::unique_ptr<BlockEntity>> block_entities;
		ColumnHeights chunk_heights;
		bool decorated;
		bool can_unload;
		std::mutex mutex;
		AABB aabb;
	};
}  // namespace Eon