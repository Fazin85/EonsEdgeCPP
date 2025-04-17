#include "chunk_renderer_container.h"

namespace Eon {
	ChunkRendererContainer::ChunkRendererContainer(Chunk& chunk, std::unique_ptr<ChunkRenderer> opaqueRenderer) : chunk(chunk), opaque_renderer(std::move(opaqueRenderer))
	{
	}

	Chunk& ChunkRendererContainer::GetChunk()
	{
		return chunk;
	}

	ChunkRenderer& ChunkRendererContainer::GetOpaqueRenderer()
	{
		return *opaque_renderer;
	}

	std::optional<std::reference_wrapper<ChunkRenderer>> ChunkRendererContainer::GetCutoutRenderer()
	{
		if (cutout_renderer.has_value()) {
			return *cutout_renderer->get();
		}

		return std::nullopt;
	}

	std::optional<std::reference_wrapper<ChunkRenderer>> ChunkRendererContainer::GetTranslucentRenderer()
	{
		if (translucent_renderer.has_value()) {
			return *translucent_renderer->get();
		}

		return std::nullopt;
	}
}
