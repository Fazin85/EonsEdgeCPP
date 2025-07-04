#include "opaque_render_pass.h"

namespace Eon
{
	OpaqueRenderPass::OpaqueRenderPass(RenderPipeline& pipeline, Framebuffer& gBuffer) : BatchedRenderPass(pipeline, "StandardOpaque", gBuffer) {}

	void OpaqueRenderPass::Begin(RenderState& renderState)
	{
		GBufferRenderPass::Begin(renderState);
	}

	void OpaqueRenderPass::Execute(RenderState& renderState)
	{
		renderState.Apply();

		BatchedRenderPass::Execute(renderState);
	}
}