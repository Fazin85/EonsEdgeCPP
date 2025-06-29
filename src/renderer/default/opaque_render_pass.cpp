#include "opaque_render_pass.h"

namespace Eon
{
	OpaqueRenderPass::OpaqueRenderPass(RenderPipeline& pipeline) : BatchedRenderPass(pipeline, "StandardOpaque") {}

	void OpaqueRenderPass::Begin(RenderState& renderState)
	{
	}

	void OpaqueRenderPass::Execute(RenderState& renderState)
	{
		renderState.Apply();

		BatchedRenderPass::Execute(renderState);
	}
}