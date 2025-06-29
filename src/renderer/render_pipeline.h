#pragma once

#include "render_pass.h"

namespace Eon
{
	class RenderPipeline
	{
	public:
		struct RenderStats
		{
			int draw_calls = 0;
			int time_ms = 0;
		};

		virtual ~RenderPipeline() = default;

		virtual void BeginFrame(Camera& camera, const glm::vec3& cameraPosition) = 0;
		virtual void Submit(std::unique_ptr<RenderCommand>& renderCommand) = 0;
		virtual void EndFrame() = 0;

		virtual RenderStats GetRenderStats() const = 0;
	};
}