#pragma once

#include "../batched_render_pass.h"

namespace Eon
{
	class OpaqueRenderPass : public BatchedRenderPass
	{
	public:
		explicit OpaqueRenderPass(RenderPipeline& pipeline, Framebuffer& framebuffer);

		void Begin(RenderState& renderState) override;
		void Execute(RenderState& renderState) override;
	};
}