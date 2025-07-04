#pragma once

#include "frame_buffer_render_pass.h"

namespace Eon
{
	class DepthSortedRenderPass : public FrameBufferRenderPass
	{
	public:
		DepthSortedRenderPass(RenderPipeline& pipeline, const std::string& name, Framebuffer& framebuffer);

		void Execute(RenderState& renderState) override;
		void End(RenderState& renderState) override;

		void Submit(RenderCommandVariant& renderCommand) override;
	};
}