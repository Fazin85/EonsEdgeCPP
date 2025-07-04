#include "translucent_render_pass.h"

namespace Eon
{
	TranslucentRenderPass::TranslucentRenderPass(RenderPipeline& pipeline) : DepthSortedRenderPass(pipeline, "StandardTranslucent")
	{
	}

	void TranslucentRenderPass::Begin(RenderState& renderState)
	{
		renderState.SetCullFace(false);
		renderState.SetDepthMask(false);
		renderState.SetBlend(true);
		//we don't set the blend function because it's set automatically to GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
		renderState.Apply();
	}
}