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
		void Clear() override;
		void Submit(RenderCommandVariant& renderCommand) override;
		void EndFrame() override;
		void Display() override;

		void SetGlobalUniform(const std::string& name, const glm::mat4& value) override;
		void SetGlobalUniform(const std::string& name, const glm::vec3& value) override;
		void SetGlobalUniform(const std::string& name, float value) override;
		void SetGlobalUniform(const std::string& name, int value) override;

		void OnShaderBound(Shader& shader) override;
		void OnTextureBound(Texture& texture) override;

		RenderStats& GetRenderStats()override;

	private:
		void ApplyGlobalUniforms(Shader& shader) const;

		std::vector<std::unique_ptr<RenderPass>> render_passes;
		RenderStats render_stats;
		RenderState render_state;
		std::unordered_map<std::string, glm::mat4> global_mat4_uniforms;
		std::unordered_map<std::string, glm::vec3> global_vec3_uniforms;
		std::unordered_map<std::string, float> global_float_uniforms;
		std::unordered_map<std::string, int> global_int_uniforms;
		std::unique_ptr<Framebuffer> g_buffer;
	};
}