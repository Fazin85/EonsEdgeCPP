#include "render_pass.h"

namespace Eon
{
	RenderPass::RenderPass(RenderPipeline& pipeline, const std::string& name) : name(name), pipeline(pipeline) {}

	void RenderPass::End(RenderState& renderState)
	{
		renderState.Reset();
	}
}