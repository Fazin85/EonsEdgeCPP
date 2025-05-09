#include "chunk_renderer_container.h"

namespace Eon
{
	ChunkRendererContainer::ChunkRendererContainer(std::shared_ptr<Chunk> chunk, std::unique_ptr<ChunkRenderer> opaqueRenderer) : chunk(chunk), opaque_renderer(std::move(opaqueRenderer))
	{
	}

	std::shared_ptr<Chunk> ChunkRendererContainer::GetChunk()
	{
		return chunk;
	}

	void ChunkRendererContainer::Setup()
	{
		opaque_renderer->Setup();

		if (cutout_renderer.has_value())
		{
			cutout_renderer->get()->Setup();
		}

		if (translucent_renderer.has_value())
		{
			translucent_renderer->get()->Setup();
		}
	}

	ChunkRenderer& ChunkRendererContainer::GetOpaqueRenderer()
	{
		return *opaque_renderer;
	}

	std::optional<std::reference_wrapper<ChunkRenderer>> ChunkRendererContainer::GetCutoutRenderer()
	{
		if (cutout_renderer.has_value())
		{
			return *cutout_renderer->get();
		}

		return std::nullopt;
	}

	std::optional<std::reference_wrapper<ChunkRenderer>> ChunkRendererContainer::GetTranslucentRenderer()
	{
		if (translucent_renderer.has_value())
		{
			return *translucent_renderer->get();
		}

		return std::nullopt;
	}

	void ChunkRendererContainer::SetCutoutRenderer(std::unique_ptr<ChunkRenderer> cutoutRenderer)
	{
		cutout_renderer = std::move(cutoutRenderer);
	}

	void ChunkRendererContainer::SetTranslucentRenderer(std::unique_ptr<ChunkRenderer> translucentRenderer)
	{
		translucent_renderer = std::move(translucentRenderer);
	}
}
