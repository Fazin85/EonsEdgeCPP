#pragma once

#include "frame_buffer_render_pass.h"

namespace Eon
{
	class BatchedRenderPass : public FrameBufferRenderPass
	{
	public:
		explicit BatchedRenderPass(RenderPipeline& pipeline, const std::string& name, Framebuffer& framebuffer);

		void Execute(RenderState& renderState) override;
		void End(RenderState& renderState) override;

		void Submit(RenderCommandVariant& renderCommand) override;
	private:
		std::unordered_map<ShaderID, std::unordered_map<TextureID, std::vector<std::reference_wrapper<RenderCommandVariant>>>> shader_batches;
	};
}