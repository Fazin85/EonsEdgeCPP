#pragma once

#include "g_buffer_render_pass.h"

namespace Eon
{
	class DepthSortedRenderPass : public GBufferRenderPass
	{
	public:
		DepthSortedRenderPass(RenderPipeline& pipeline, const std::string& name, Framebuffer& gBuffer);

		void Execute(RenderState& renderState) override;
		void End(RenderState& renderState) override;

		void Submit(RenderCommandVariant& renderCommand) override;
	};
}