#include "default_render_pipeline.h"
#include "opaque_render_pass.h"

namespace Eon
{
	DefaultRenderPipeline::DefaultRenderPipeline()
	{
		render_passes.emplace_back(std::make_unique<OpaqueRenderPass>());
	}

	void DefaultRenderPipeline::BeginFrame(Camera& camera, const glm::vec3& cameraPosition)
	{
		this->camera = &camera;
		camera_position = cameraPosition;
	}

	void DefaultRenderPipeline::Submit(std::unique_ptr<RenderCommand>& renderCommand)
	{
		render_passes[0]->Submit(renderCommand);
	}

	void DefaultRenderPipeline::EndFrame()
	{
		if (!camera)
		{
			return;
		}

		for (const auto& pass : render_passes)
		{
			pass->Begin(render_state);
			pass->Execute(render_state, *camera, camera_position);
			pass->End(render_state);
		}
	}

	RenderPipeline::RenderStats DefaultRenderPipeline::GetRenderStats() const
	{
		return RenderStats();
	}
}
