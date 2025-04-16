#pragma once

#include "aabb_chunk_renderer.h"

namespace Eon {
	class AABBChunkRendererProvider {
	public:
		virtual ~AABBChunkRendererProvider() = default;
		virtual std::unique_ptr<AABBChunkRenderer> ProvideRenderer(ChunkPosition inChunkPosition) = 0;
	};
}