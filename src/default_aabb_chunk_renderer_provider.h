#pragma once

#include "aabb_chunk_renderer_provider.h"
#include "directions.h"
#include "block_face_texture.h"
#include "level.h"

namespace Eon {
	class DefaultAABBChunkRendererProvider : public AABBChunkRendererProvider {
	public:
		DefaultAABBChunkRendererProvider(Level& level);

		std::unique_ptr<AABBChunkRenderer> ProvideRenderer(ChunkPosition inChunkPosition) override;
	private:
		void AddFace(ChunkMeshConstructionData& meshData, const glm::ivec3& blockPosition, BlockType blockType, Directions direction);
		std::array<unsigned char, 12> GetFaceDataFromDirection(Directions dir);
		void AddIndices(ChunkMeshConstructionData& meshData, int count);
		BlockFaceTexture GetTextureId(BlockType blockType, Directions faceDirection);

		Level& level;
	};
}