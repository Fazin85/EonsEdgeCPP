#pragma once

#include "chunk_renderer.h"
#include "chunk.h"

namespace Eon {
	class ChunkRendererContainer {
	public:
		ChunkRendererContainer(Chunk& chunk, std::unique_ptr<ChunkRenderer> opaqueRenderer);

		Chunk& GetChunk();
		ChunkRenderer& GetOpaqueRenderer();
		std::optional<std::reference_wrapper<ChunkRenderer>> GetCutoutRenderer();
		std::optional<std::reference_wrapper<ChunkRenderer>> GetTranslucentRenderer();

	private:
		Chunk& chunk;
		std::unique_ptr<ChunkRenderer> opaque_renderer;
		std::optional<std::unique_ptr<ChunkRenderer>> cutout_renderer;
		std::optional<std::unique_ptr<ChunkRenderer>> translucent_renderer;
	};
}