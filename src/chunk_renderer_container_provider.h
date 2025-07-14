#pragma once

#include "chunk_renderer_container.h"

namespace Eon
{
	class Chunk;
	class Block;
	class BlockProvider;

	struct ChunkRendererCreationContext
	{
		std::shared_ptr<Chunk> chunk;
		int lod = 1;
		int chunk_x = 0;
		int chunk_z = 0;
		BlockProvider& blockProvider;
	};

	class ChunkRendererContainerProvider
	{
	public:
		virtual ~ChunkRendererContainerProvider() = default;
		virtual std::unique_ptr<ChunkRendererContainer> ProvideRenderer(ChunkRendererCreationContext& creationContext) = 0;
	};
}