#pragma once

#include "../render_pipeline.h"
#include "../render_pass.h"

namespace Eon
{
	class DefaultRenderPipeline : public RenderPipeline
	{
	public:
		DefaultRenderPipeline();

		void BeginFrame() override;
		void Submit(std::unique_ptr<RenderCommand> renderCommand) override;
		void EndFrame() override;

		void SetGlobalUniform(const std::string& name, const glm::mat4& value) override;
		void SetGlobalUniform(const std::string& name, const glm::vec3& value) override;
		void SetGlobalUniform(const std::string& name, float value) override;

		void ApplyGlobalUniforms(Shader& shader) override;

		RenderStats GetRenderStats() const override;

	private:
		std::vector<std::unique_ptr<RenderPass>> render_passes;
		RenderStats render_stats;
		RenderState render_state;
		std::unordered_map<std::string, glm::mat4> global_mat4_uniforms;
		std::unordered_map<std::string, glm::vec3> global_vec3_uniforms;
		std::unordered_map<std::string, float> global_float_uniforms;
	};
}