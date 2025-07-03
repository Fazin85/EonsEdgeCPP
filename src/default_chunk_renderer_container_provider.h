#pragma once

#include "chunk_renderer_container_provider.h"
#include "chunk_mesh_construction_data.h"
#include "directions.h"
#include "block_face_texture.h"
#include "level.h"

namespace Eon
{
	class BlockTextureProvider;

	class DefaultChunkRendererContainerProvider : public ChunkRendererContainerProvider
	{
	public:
		explicit DefaultChunkRendererContainerProvider(Level& level, BlockTextureProvider& uvProvider);

		std::unique_ptr<ChunkRendererContainer> ProvideRenderer(ChunkPosition inChunkPosition) override;
	private:	
		std::unique_ptr<Mesh> CreateMesh(ChunkMeshConstructionData& meshData);

		int CalculateLowestPoint(const ChunkPosition& position, int middleLowest);

		ChunkMeshConstructionData& GetMeshData(Block block, ChunkMeshConstructionData& opaqueMeshData, ChunkMeshConstructionData& cutoutMeshData, ChunkMeshConstructionData& translucentMeshData);

		Level& level;
		BlockTextureProvider& uvProvider;
	};
}