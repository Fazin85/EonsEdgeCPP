#pragma once

#include "render_pass.h"

namespace Eon
{
	class BatchedRenderPass : public RenderPass
	{
	public:
		explicit BatchedRenderPass(RenderPipeline& pipeline, const std::string& name);

		void Execute(RenderState& renderState) override;
		void End(RenderState& renderState) override;

		void Submit(std::unique_ptr<RenderCommand>& renderCommand) override;
	private:
		void BindShader(ShaderID shaderId, RenderState& renderState);
		void BindTexture(TextureID textureId);

		ShaderID last_bound_shader;
		TextureID last_bound_texture;
		std::unordered_map<ShaderID, std::unordered_map<TextureID, std::vector<std::unique_ptr<RenderCommand>>>> shader_batches;
	};
}