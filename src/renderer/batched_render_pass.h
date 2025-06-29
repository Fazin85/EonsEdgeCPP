#pragma once

#include "render_pass.h"

namespace Eon
{
	class BatchedRenderPass : public RenderPass
	{
	public:
		explicit BatchedRenderPass(const std::string& name);

		void Execute(RenderState& renderState, Camera& camera, glm::vec3 cameraPosition) override;
		void End(RenderState& renderState) override;

		void Submit(std::unique_ptr<RenderCommand>& renderCommand) override;
	private:
		void BindShader(ShaderID shaderId) const;
		void BindTexture(TextureID textureId) const;

		std::unordered_map<ShaderID, std::unordered_map<TextureID, std::vector<std::unique_ptr<RenderCommand>>>> shader_batches;
	};
}