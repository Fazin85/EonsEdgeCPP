#pragma once

#include "render_pass.h"

namespace Eon
{
	class GBufferRenderPass : public RenderPass
	{
	public:
		GBufferRenderPass(RenderPipeline& pipeline, const std::string& name, Framebuffer& gBuffer);

		void Begin(RenderState& renderState) override;

		Framebuffer& GetGBuffer() const;

	private:
		Framebuffer& g_buffer;
	};
}