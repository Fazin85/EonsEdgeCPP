#pragma once

#include "chunk_renderer_container_provider.h"
#include "directions.h"
#include "block_face_texture.h"
#include "level.h"

namespace Eon {
	class DefaultChunkRendererContainerProvider : public ChunkRendererContainerProvider {
	public:
		DefaultChunkRendererContainerProvider(Level& level);

		std::unique_ptr<ChunkRendererContainer> ProvideRenderer(ChunkPosition inChunkPosition) override;
	private:
		int CalculateLowestPoint(ChunkPosition& position, int middleLowest);

		ChunkMeshConstructionData& GetMeshData(Block block, ChunkMeshConstructionData& opaqueMeshData, ChunkMeshConstructionData& cutoutMeshData, ChunkMeshConstructionData& translucentMeshData);

		void AddFace(ChunkMeshConstructionData& meshData, const glm::ivec3& blockPosition, BlockType blockType, Directions direction);
		std::array<unsigned char, 12> GetFaceDataFromDirection(Directions dir);
		void AddIndices(ChunkMeshConstructionData& meshData);
		BlockFaceTexture GetTextureId(BlockType blockType, Directions faceDirection);

		Level& level;
	};
}