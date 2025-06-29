#include "render_pass.h"

namespace Eon
{
	RenderPass::RenderPass(const std::string& name) : name(name) {}
	
	void RenderPass::End(RenderState& renderState)
	{
		renderState.Reset();
	}
}