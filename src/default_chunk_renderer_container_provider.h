#pragma once

#include "chunk_renderer_container_provider.h"
#include "directions.h"
#include "block_face_texture.h"
#include "level.h"

namespace Eon {
	class DefaultChunkRendererContainerProvider : public ChunkRendererContainerProvider {
	public:
		explicit DefaultChunkRendererContainerProvider(Level& level);

		std::unique_ptr<ChunkRendererContainer> ProvideRenderer(ChunkPosition inChunkPosition) override;
	private:
		int CalculateLowestPoint(const ChunkPosition& position, int middleLowest);

		ChunkMeshConstructionData& GetMeshData(Block block, ChunkMeshConstructionData& opaqueMeshData, ChunkMeshConstructionData& cutoutMeshData, ChunkMeshConstructionData& translucentMeshData);

		void AddFace(ChunkMeshConstructionData& meshData, const glm::ivec3& blockPosition, const Block& block, Directions direction) const;
		std::array<unsigned char, 12> GetFaceDataFromDirection(Directions dir) const;
		void AddIndices(ChunkMeshConstructionData& meshData) const;
		BlockFaceTexture GetTextureId(BlockType blockType, Directions faceDirection) const;

		Level& level;
	};
}