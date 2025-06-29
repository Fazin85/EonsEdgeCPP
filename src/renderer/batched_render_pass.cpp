#include "batched_render_pass.h"
#include "../assert.h"

namespace Eon
{
	BatchedRenderPass::BatchedRenderPass(RenderPipeline& pipeline, const std::string& name) : RenderPass(pipeline, name) {}

	void BatchedRenderPass::Submit(std::unique_ptr<RenderCommand>& renderCommand)
	{
		Material material = renderCommand->GetMaterial();

		ShaderID shaderId = material.GetShader();
		TextureID textureId = material.GetTexture();

		EON_ASSERT(shaderId.IsValid() && textureId.IsValid(), "Invalid material");

		shader_batches[shaderId][textureId].emplace_back(std::move(renderCommand));
	}

	void BatchedRenderPass::Execute(RenderState& renderState)
	{
		for (const auto& [shaderId, textureMap] : shader_batches)
		{
			BindShader(shaderId, renderState);

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

	void BatchedRenderPass::BindShader(ShaderID shaderId, RenderState& renderState)
	{
		if (shaderId != last_bound_shader)
		{
			auto shader = shaderId.Get<Shader>();

			EON_ASSERT(shader.IsValid(), "Invalid shader");

			shader->Bind();
			pipeline.ApplyGlobalUniforms(*shader);
			pipeline.GetRenderStats().shader_binds++;
			renderState.shader = &*shader;
			last_bound_shader = shaderId;
		}
	}

	void BatchedRenderPass::BindTexture(TextureID textureId)
	{
		if (textureId != last_bound_texture)
		{
			auto texture = textureId.Get<Texture>();

			EON_ASSERT(texture.IsValid(), "Invalid texture");

			texture->Bind();
			pipeline.GetRenderStats().texture_binds++;
			last_bound_texture = textureId;
		}
	}

	void BatchedRenderPass::End(RenderState& renderState)
	{
		RenderPass::End(renderState);
		shader_batches.clear();
		last_bound_shader = AssetID::INVALID_ASSET_ID;
		last_bound_texture = AssetID::INVALID_ASSET_ID;
	}
}