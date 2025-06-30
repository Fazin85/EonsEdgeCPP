#include "batched_render_pass.h"
#include "../assert.h"

namespace Eon
{
	BatchedRenderPass::BatchedRenderPass(RenderPipeline& pipeline, const std::string& name) : RenderPass(pipeline, name) {}

	void BatchedRenderPass::Submit(RenderCommandVariant& renderCommand)
	{
		Material material = GetRenderCommand(renderCommand).GetMaterial();

		ShaderID shaderId = material.GetShader();
		TextureID textureId = material.GetTexture();

		EON_ASSERT(shaderId.IsValid() && textureId.IsValid(), "Invalid material");

		shader_batches[shaderId][textureId].emplace_back(renderCommand);
	}

	void BatchedRenderPass::Execute(RenderState& renderState)
	{
		for (const auto& [shaderId, textureMap] : shader_batches)
		{
			if (renderState.SetShader(shaderId))
			{
				renderState.Apply();
				pipeline.OnShaderBound(*shaderId.Get<Shader>());
			}

			for (const auto& [textureId, commands] : textureMap)
			{
				if (renderState.SetTexture(textureId))
				{
					renderState.Apply();
					pipeline.OnTextureBound(*textureId.Get<Texture>());
				}

				for (auto& command : commands)
				{
					GetRenderCommand(command).Execute(renderState);
				}
			}
		}
	}

	void BatchedRenderPass::End(RenderState& renderState)
	{
		RenderPass::End(renderState);
		shader_batches.clear();
	}
}