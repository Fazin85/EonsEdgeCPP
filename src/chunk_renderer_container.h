#pragma once

#include "chunk_renderer.h"
#include "chunk.h"

namespace Eon {
	class ChunkRendererContainer {
	public:
		ChunkRendererContainer(std::shared_ptr<Chunk> chunk, std::unique_ptr<ChunkRenderer> opaqueRenderer);

		std::shared_ptr<Chunk> GetChunk();

		void Setup();

		ChunkRenderer& GetOpaqueRenderer();
		std::optional<std::reference_wrapper<ChunkRenderer>> GetCutoutRenderer();
		std::optional<std::reference_wrapper<ChunkRenderer>> GetTranslucentRenderer();

		void SetCutoutRenderer(std::unique_ptr<ChunkRenderer> cutoutRenderer);
		void SetTranslucentRenderer(std::unique_ptr<ChunkRenderer> translucentRenderer);

	private:
		std::shared_ptr<Chunk> chunk;
		std::unique_ptr<ChunkRenderer> opaque_renderer;
		std::optional<std::unique_ptr<ChunkRenderer>> cutout_renderer;
		std::optional<std::unique_ptr<ChunkRenderer>> translucent_renderer;
	};
}