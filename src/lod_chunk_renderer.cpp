#include "lod_chunk_renderer.h"
#include "log.h"

namespace Eon
{
	LODChunkRenderer::LODChunkRenderer(Chunk& chunk, std::unique_ptr<ChunkRenderer> chunkRenderer) : chunk(chunk), lods(std::move(chunkRenderer)), aabb(glm::vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH))
	{
		aabb.Update(glm::vec3(chunk.Position().x, 0, chunk.Position().z));
	}

	LODChunkRenderer::~LODChunkRenderer()
	{
	}

	void LODChunkRenderer::Render(unsigned int lod)
	{
		lods->Render();
	}

	void LODChunkRenderer::Setup()
	{
		lods->Setup();
	}

	ChunkRenderer* LODChunkRenderer::GetLodRenderer(unsigned int lod)
	{
		//if (lod > CHUNK_MAX_LOD / 2 || lod < 0)
		//{
		//	EON_ERROR("Tried to get lod renderer with lod: " + std::to_string(lod));
		//	return nullptr;
		//}

		return lods.get();
	}

	AABB& LODChunkRenderer::GetAABB()
	{
		return aabb;
	}

	Chunk& LODChunkRenderer::GetChunk()
	{
		return chunk;
	}
}
