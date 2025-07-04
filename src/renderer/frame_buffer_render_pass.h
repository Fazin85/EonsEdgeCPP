#pragma once

#include "render_pass.h"

namespace Eon
{
	class FrameBufferRenderPass : public RenderPass
	{
	public:
		FrameBufferRenderPass(RenderPipeline& pipeline, const std::string& name, Framebuffer& framebuffer);

		void Begin(RenderState& renderState) override;

		Framebuffer& GetFramebuffer() const;

	private:
		Framebuffer& frame_buffer;
	};
}