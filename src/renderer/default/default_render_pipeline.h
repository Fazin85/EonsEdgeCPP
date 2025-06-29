#pragma once

#include "../render_pipeline.h"

namespace Eon
{
	class DefaultRenderPipeline : public RenderPipeline
	{
	public:
		DefaultRenderPipeline();

		void BeginFrame(Camera& camera, const glm::vec3& cameraPosition) override;
		void Submit(std::unique_ptr<RenderCommand>& renderCommand) override;
		void EndFrame() override;

		RenderStats GetRenderStats() const override;

	private:
		std::vector<std::unique_ptr<RenderPass>> render_passes;
		Camera* camera;
		glm::vec3 camera_position;
		RenderStats render_stats;
		RenderState render_state;
	};
}