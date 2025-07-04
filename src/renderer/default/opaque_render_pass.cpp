#include "opaque_render_pass.h"
#include "../../framebuffer.h"

namespace Eon
{
	OpaqueRenderPass::OpaqueRenderPass(RenderPipeline& pipeline, Framebuffer& framebuffer) : BatchedRenderPass(pipeline, "StandardOpaque", framebuffer) {}

	void OpaqueRenderPass::Begin(RenderState& renderState)
	{
		FrameBufferRenderPass::Begin(renderState);

		const Framebuffer& frameBuffer = GetFramebuffer();
		renderState.SetGBufferTextures(frameBuffer.GetColorAttachment(0), frameBuffer.GetColorAttachment(1), frameBuffer.GetColorAttachment(2));
	}

	void OpaqueRenderPass::Execute(RenderState& renderState)
	{
		renderState.Apply();

		BatchedRenderPass::Execute(renderState);
	}
}