#pragma once

#include "render_command.h"
#include "../camera.h"

namespace Eon
{
	class RenderPipeline
	{
	public:
		struct RenderStats
		{
			int draw_calls = 0;
			int shader_binds = 0;
			int texture_binds = 0;
			int time_ms = 0;

			void Reset();
		};

		virtual ~RenderPipeline() = default;

		virtual void BeginFrame() = 0;
		virtual void Submit(std::unique_ptr<RenderCommand> renderCommand) = 0;
		virtual void EndFrame() = 0;

		virtual void ApplyGlobalUniforms(Shader& shader) = 0;

		virtual void SetGlobalUniform(const std::string& name, const glm::mat4& value) = 0;
		virtual void SetGlobalUniform(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetGlobalUniform(const std::string& name, float value) = 0;

		virtual RenderStats& GetRenderStats() = 0;
	};
}