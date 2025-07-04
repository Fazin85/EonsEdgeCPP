#include "frame_buffer_render_pass.h"
#include "../framebuffer.h"

namespace Eon
{
	FrameBufferRenderPass::FrameBufferRenderPass(RenderPipeline& pipeline, const std::string& name, Framebuffer& framebuffer) : RenderPass(pipeline, name), frame_buffer(framebuffer)
	{
	}

	void FrameBufferRenderPass::Begin(RenderState& renderState)
	{
		renderState.SetFramebuffer(&frame_buffer);
		renderState.Apply();
	}

	Framebuffer& FrameBufferRenderPass::GetFramebuffer() const
	{
		return frame_buffer;
	}
}