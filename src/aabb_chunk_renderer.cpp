#include "aabb_chunk_renderer.h"
#include "log.h"

namespace Eon
{
	AABBChunkRenderer::AABBChunkRenderer(Chunk& chunk, std::unique_ptr<ChunkRenderer> chunkRenderer) : chunk(chunk), lods(std::move(chunkRenderer))
	{
	}

	void AABBChunkRenderer::Render(unsigned int lod)
	{
		lods->Render();
	}

	void AABBChunkRenderer::Setup()
	{
		lods->Setup();
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
