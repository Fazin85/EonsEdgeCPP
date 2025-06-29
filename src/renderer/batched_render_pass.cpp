#include "batched_render_pass.h"
#include "../assert.h"

namespace Eon
{
	BatchedRenderPass::BatchedRenderPass(const std::string& name) : RenderPass(name) {}

	void BatchedRenderPass::Submit(std::unique_ptr<RenderCommand>& renderCommand)
	{
		Material material = renderCommand->GetMaterial();

		ShaderID shaderId = material.GetShader();
		TextureID textureId = material.GetTexture();

		EON_ASSERT(shaderId.IsValid() && textureId.IsValid(), "Invalid material");

		shader_batches[shaderId][textureId].emplace_back(std::move(renderCommand));
	}

	void BatchedRenderPass::Execute(RenderState& renderState, Camera& camera, glm::vec3 cameraPosition)
	{
		for (const auto& [shaderId, textureMap] : shader_batches)
		{
			BindShader(shaderId);

			for (const auto& [textureId, commands] : textureMap)
			{
				BindTexture(textureId);

				for (auto& command : commands)
				{
					command->Execute(renderState);
				}
			}
		}
	}

	void BatchedRenderPass::BindShader(ShaderID shaderId) const
	{
		static ShaderID lastBoundShader;
		if (shaderId != lastBoundShader)
		{
			auto shader = shaderId.Get<Shader>();

			EON_ASSERT(shader.IsValid(), "Invalid shader");

			shader->Bind();
			lastBoundShader = shaderId;
		}
	}

	void BatchedRenderPass::BindTexture(TextureID textureId) const
	{
		static TextureID lastBoundTexture;
		if (textureId != lastBoundTexture)
		{
			auto texture = textureId.Get<Texture>();

			EON_ASSERT(texture.IsValid(), "Invalid texture");

			texture->Bind();
			lastBoundTexture = textureId;
		}
	}

	void BatchedRenderPass::End(RenderState& renderState)
	{
		RenderPass::End(renderState);
		shader_batches.clear();
	}
}