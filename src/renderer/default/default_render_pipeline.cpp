#include "default_render_pipeline.h"
#include "opaque_render_pass.h"

namespace Eon
{
	DefaultRenderPipeline::DefaultRenderPipeline()
	{
		render_passes.emplace_back(std::make_unique<OpaqueRenderPass>(*this));
	}

	void DefaultRenderPipeline::BeginFrame()
	{
		render_stats.Reset();
	}

	void DefaultRenderPipeline::Submit(std::unique_ptr<RenderCommand> renderCommand)
	{
		render_passes[0]->Submit(renderCommand);
	}

	void DefaultRenderPipeline::EndFrame()
	{
		for (const auto& pass : render_passes)
		{
			pass->Begin(render_state);
			pass->Execute(render_state);
			pass->End(render_state);
		}
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

	void DefaultRenderPipeline::ApplyGlobalUniforms(Shader& shader)
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
	}

	RenderPipeline::RenderStats& DefaultRenderPipeline::GetRenderStats()
	{
		return render_stats;
	}
}
