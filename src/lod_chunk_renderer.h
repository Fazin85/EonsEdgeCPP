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
		LODChunkRenderer(Chunk* chunk, std::vector<ChunkRenderer*>& chunkRenderers);
		~LODChunkRenderer();
		void Render(unsigned int lod);
		void Setup();
		ChunkRenderer* GetLodRenderer(unsigned int lod);
		AABB& GetAABB();
		Chunk& GetChunk();
		std::vector<eon_chunk_mesh_data> GetAllMeshDataFromGpu();

	private:
		std::array<ChunkRenderer*, CHUNK_MAX_LOD / 2> lods;
		AABB aabb;
		Chunk* chunk;
	};
}