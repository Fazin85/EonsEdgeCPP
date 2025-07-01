#pragma once

//#include "chunk_renderer.h"
#include "chunk.h"

namespace Eon
{
	class Mesh;

	class ChunkRendererContainer
	{
	public:
		ChunkRendererContainer(std::shared_ptr<Chunk> chunk, std::unique_ptr<Mesh> opaqueRenderer);

		std::shared_ptr<Chunk> GetChunk();

		void Setup();

		Mesh& GetOpaqueRenderer();
		std::optional<std::reference_wrapper<Mesh>> GetCutoutRenderer();
		std::optional<std::reference_wrapper<Mesh>> GetTranslucentRenderer();

		void SetCutoutRenderer(std::unique_ptr<Mesh> cutoutRenderer);
		void SetTranslucentRenderer(std::unique_ptr<Mesh> translucentRenderer);

	private:
		std::shared_ptr<Chunk> chunk;
		std::unique_ptr<Mesh> opaque_renderer;
		std::optional<std::unique_ptr<Mesh>> cutout_renderer;
		std::optional<std::unique_ptr<Mesh>> translucent_renderer;
	};
}