#pragma once

#include "../frame_buffer_render_pass.h"

namespace Eon 
{
	class RenderPassSSR : public FrameBufferRenderPass
	{
	public:
		explicit RenderPassSSR(RenderPipeline& pipeline, Framebuffer& framebuffer);

		void Execute(RenderState& renderState) override;
		void End(RenderState& renderState) override;

	private:
		ShaderID ssr_shader;
		std::unique_ptr<PositionTextureMesh> fullscreen_quad = nullptr;
	};
}