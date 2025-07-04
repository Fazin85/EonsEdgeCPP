#include "depth_sorted_render_pass.h"
#include "../assert.h"

namespace Eon
{
	DepthSortedRenderPass::DepthSortedRenderPass(RenderPipeline& pipeline, const std::string& name) : RenderPass(pipeline, name)
	{
	}

	void DepthSortedRenderPass::Execute(RenderState& renderState)
	{
		std::sort(render_commands.begin(), render_commands.end(), [](RenderCommandVariant& a, RenderCommandVariant& b) { return GetRenderCommand(a).GetDepth() > GetRenderCommand(b).GetDepth(); });

		for (const auto& command : render_commands)
		{
			auto& renderCommand = GetRenderCommand(command);
			auto shaderId = renderCommand.GetMaterial().GetShader();
			auto textureId = renderCommand.GetMaterial().GetTexture();

			if (renderState.SetShader(shaderId))
			{
				renderState.Apply();
				pipeline.OnShaderBound(*shaderId.Get<Shader>());
			}

			if (renderState.SetTexture(textureId))
			{
				renderState.Apply();
				pipeline.OnTextureBound(*textureId.Get<Texture>());
			}

			renderCommand.GetMaterial().PreRender(renderState);
			renderCommand.Execute(renderState);
		}
	}

	void DepthSortedRenderPass::End(RenderState& renderState)
	{
		render_commands.clear();
		RenderPass::End(renderState);
	}

	void DepthSortedRenderPass::Submit(RenderCommandVariant& renderCommand)
	{
		Material material = GetRenderCommand(renderCommand).GetMaterial();

		ShaderID shaderId = material.GetShader();
		TextureID textureId = material.GetTexture();

		EON_ASSERT(shaderId.IsValid() && textureId.IsValid(), "Invalid material");

		render_commands.emplace_back(renderCommand);
	}
}