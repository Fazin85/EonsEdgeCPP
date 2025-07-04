#include "default_render_pipeline.h"
#include "opaque_render_pass.h"
#include "translucent_render_pass.h"

namespace Eon
{
	DefaultRenderPipeline::DefaultRenderPipeline()
	{
		render_passes.emplace_back(std::make_unique<OpaqueRenderPass>(*this));
		render_passes.emplace_back(std::make_unique<TranslucentRenderPass>(*this));
	}

	void DefaultRenderPipeline::BeginFrame()
	{
		render_stats.Reset();
	}

	void DefaultRenderPipeline::Submit(RenderCommandVariant& renderCommand)
	{
		const auto& command = GetRenderCommand(renderCommand);

		switch (command.GetMaterial().GetTransparencyType())
		{
		case TransparencyType::Opaque:
			render_passes[0]->Submit(renderCommand);
			break;
		case TransparencyType::Cutout:
			break;
		case TransparencyType::Transparent:
			render_passes[1]->Submit(renderCommand);
			break;
		}
	}

	void DefaultRenderPipeline::EndFrame()
	{
		for (const auto& pass : render_passes)
		{
			pass->Begin(render_state);
			pass->Execute(render_state);
			pass->End(render_state);
		}

		global_mat4_uniforms.clear();
		global_vec3_uniforms.clear();
		global_float_uniforms.clear();
		global_int_uniforms.clear();
	}

	void DefaultRenderPipeline::SetGlobalUniform(const std::string& name, const glm::mat4& value)
	{
		global_mat4_uniforms[name] = value;
	}

	void DefaultRenderPipeline::SetGlobalUniform(const std::string& name, const glm::vec3& value)
	{
		global_vec3_uniforms[name] = value;
	}

	void DefaultRenderPipeline::SetGlobalUniform(const std::string& name, float value)
	{
		global_float_uniforms[name] = value;
	}

	void DefaultRenderPipeline::SetGlobalUniform(const std::string& name, int value)
	{
		global_int_uniforms[name] = value;
	}

	void DefaultRenderPipeline::OnShaderBound(Shader& shader)
	{
		ApplyGlobalUniforms(shader);
		render_stats.shader_binds++;
	}

	void DefaultRenderPipeline::OnTextureBound(Texture& texture)
	{
		render_stats.texture_binds++;
	}

	void DefaultRenderPipeline::ApplyGlobalUniforms(Shader& shader) const
	{
		for (const auto& [name, value] : global_mat4_uniforms)
		{
			shader.UniformMatrix4(name, value);
		}

		for (const auto& [name, value] : global_vec3_uniforms)
		{
			shader.UniformFVec3(name, value);
		}

		for (const auto& [name, value] : global_float_uniforms)
		{
			shader.UniformFloat(name, value);
		}

		for (const auto& [name, value] : global_int_uniforms)
		{
			shader.UniformInt1(name, value);
		}
	}

	RenderPipeline::RenderStats& DefaultRenderPipeline::GetRenderStats()
	{
		return render_stats;
	}
}
