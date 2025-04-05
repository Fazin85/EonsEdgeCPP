#pragma once

#include "aabb.h"
#include "chunk.h"
#include "chunk_renderer.h"
#include <array>
#include <memory>
#include <vector>

namespace Eon
{
	class LODChunkRenderer
	{
	public:
		LODChunkRenderer(Chunk& chunk, std::unique_ptr<ChunkRenderer> chunkRenderer);
		~LODChunkRenderer();
		void Render(unsigned int lod);
		void Setup();
		ChunkRenderer* GetLodRenderer(unsigned int lod);
		AABB& GetAABB();
		Chunk& GetChunk();

	private:
		std::unique_ptr<ChunkRenderer> lods;
		AABB aabb;
		Chunk& chunk;
	};
}