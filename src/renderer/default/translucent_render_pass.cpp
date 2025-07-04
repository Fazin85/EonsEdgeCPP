#include "translucent_render_pass.h"
#include "../../framebuffer.h"

namespace Eon
{
	TranslucentRenderPass::TranslucentRenderPass(RenderPipeline& pipeline, Framebuffer& framebuffer) : DepthSortedRenderPass(pipeline, "StandardTranslucent", framebuffer)
	{
	}

	void TranslucentRenderPass::Begin(RenderState& renderState)
	{
		FrameBufferRenderPass::Begin(renderState);

		const Framebuffer& frameBuffer = GetFramebuffer();
		renderState.SetGBufferTextures(frameBuffer.GetColorAttachment(0), frameBuffer.GetColorAttachment(1), frameBuffer.GetColorAttachment(2));

		renderState.SetCullFace(false);
		renderState.SetDepthMask(false);
		renderState.SetBlend(true);
		//we don't set the blend function because it's set automatically to GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
		renderState.Apply();
	}
}