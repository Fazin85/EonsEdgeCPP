#pragma once

#include "chunk_renderer_container.h"

namespace Eon {
	class ChunkRendererContainerProvider {
	public:
		virtual ~ChunkRendererContainerProvider() = default;
		virtual std::unique_ptr<ChunkRendererContainer> ProvideRenderer(ChunkPosition inChunkPosition) = 0;
	};
}