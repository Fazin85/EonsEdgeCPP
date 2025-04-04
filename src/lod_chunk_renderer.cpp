#include "lod_chunk_renderer.h"
#include "log.h"

namespace Eon
{
	LODChunkRenderer::LODChunkRenderer(Chunk* chunk, std::vector<ChunkRenderer*>& chunkRenderers) : aabb(glm::vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH)), lods{}
	{
		this->chunk = chunk;

		//if (chunkRenderers.size() != CHUNK_MAX_LOD / 2)
		//{
		//	EON_ERROR("Tried to create LODChunkRenderer with: " + std::to_string(chunkRenderers.size()) + " lods");
		//	return;
		//}

		aabb.Update(glm::vec3(chunk->Position().x * CHUNK_WIDTH, 0, chunk->Position().z * CHUNK_WIDTH));

		for (int lod = 0; lod < CHUNK_MAX_LOD / 2; lod++)
		{
			lods[lod] = chunkRenderers[lod];
		}
	}

	LODChunkRenderer::~LODChunkRenderer()
	{
		for (int lod = 0; lod < CHUNK_MAX_LOD / 2; lod++)
		{
			if (lods[lod] != nullptr)
			{
				delete lods[lod];
				lods[lod] = nullptr;
			}
		}
	}

	void LODChunkRenderer::Render(unsigned int lod)
	{
		//if (lod > CHUNK_MAX_LOD / 2 || lod < 0)
		//{
		//	EON_ERROR("Tried to render chunk with lod: " + std::to_string(lod));
		//	return;
		//}

		lods[lod]->Render();
	}

	void LODChunkRenderer::Setup()
	{
		for (int lod = 0; lod < CHUNK_MAX_LOD / 2; lod++)
		{
			lods[lod]->Setup();
		}
	}

	ChunkRenderer* LODChunkRenderer::GetLodRenderer(unsigned int lod)
	{
		//if (lod > CHUNK_MAX_LOD / 2 || lod < 0)
		//{
		//	EON_ERROR("Tried to get lod renderer with lod: " + std::to_string(lod));
		//	return nullptr;
		//}

		return lods[lod];
	}

	AABB& LODChunkRenderer::GetAABB()
	{
		return aabb;
	}

	Chunk& LODChunkRenderer::GetChunk()
	{
		return *chunk;
	}
}
