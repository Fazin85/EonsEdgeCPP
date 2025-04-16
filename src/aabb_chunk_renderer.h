#pragma once

#include "aabb.h"
#include "chunk.h"
#include "chunk_renderer.h"
#include <array>
#include <memory>
#include <vector>

namespace Eon
{
	class AABBChunkRenderer
	{
	public:
		AABBChunkRenderer(Chunk& chunk, std::unique_ptr<ChunkRenderer> chunkRenderer);
		void Render(unsigned int lod);
		void Setup();
		AABB& GetAABB();
		Chunk& GetChunk();
		ChunkRenderer& GetBaseChunkRenderer();

	private:
		std::unique_ptr<ChunkRenderer> lods;
		AABB aabb;
		Chunk& chunk;
	};
}