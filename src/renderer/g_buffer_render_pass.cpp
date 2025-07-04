#include "g_buffer_render_pass.h"
#include "../framebuffer.h"

namespace Eon
{
	GBufferRenderPass::GBufferRenderPass(RenderPipeline& pipeline, const std::string& name, Framebuffer& gBuffer) : RenderPass(pipeline, name), g_buffer(gBuffer)
	{
	}

	void GBufferRenderPass::Begin(RenderState& renderState)
	{
		renderState.SetFramebuffer(&g_buffer);
		renderState.Apply();
	}

	Framebuffer& GBufferRenderPass::GetGBuffer() const
	{
		return g_buffer;
	}
}