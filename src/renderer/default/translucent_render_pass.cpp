#include "translucent_render_pass.h"

namespace Eon
{
	TranslucentRenderPass::TranslucentRenderPass(RenderPipeline& pipeline, Framebuffer& gBuffer) : DepthSortedRenderPass(pipeline, "StandardTranslucent", gBuffer)
	{
	}

	void TranslucentRenderPass::Begin(RenderState& renderState)
	{
		GBufferRenderPass::Begin(renderState);

		renderState.SetCullFace(false);
		renderState.SetDepthMask(false);
		renderState.SetBlend(true);
		//we don't set the blend function because it's set automatically to GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
		renderState.Apply();
	}
}