#include "aabb_chunk_renderer.h"
#include "log.h"

namespace Eon
{
	AABBChunkRenderer::AABBChunkRenderer(Chunk& chunk, std::unique_ptr<ChunkRenderer> chunkRenderer) : chunk(chunk), lods(std::move(chunkRenderer)), aabb(glm::vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH))
	{
		aabb.Update(glm::vec3(chunk.Position().x, 0, chunk.Position().z));
	}

	void AABBChunkRenderer::Render(unsigned int lod)
	{
		lods->Render();
	}

	void AABBChunkRenderer::Setup()
	{
		lods->Setup();
	}

	AABB& AABBChunkRenderer::GetAABB()
	{
		return aabb;
	}

	Chunk& AABBChunkRenderer::GetChunk()
	{
		return chunk;
	}

	ChunkRenderer& AABBChunkRenderer::GetBaseChunkRenderer()
	{
		return *lods;
	}
}
