#pragma once

#include "../frame_buffer_render_pass.h"

namespace Eon
{
	class SSRRenderPass : public FrameBufferRenderPass
	{
	public:
		explicit SSRRenderPass(RenderPipeline& pipeline, Framebuffer& framebuffer);

		void Execute(RenderState& renderState) override;
		void End(RenderState& renderState) override;

		void Submit(RenderCommandVariant& renderCommand) override;

	private:
		ShaderID ssr_shader;
		std::unique_ptr<PositionTextureMesh> fullscreen_quad = nullptr;
	};
}