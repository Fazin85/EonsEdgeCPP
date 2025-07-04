#pragma once

#include "command/render_command_variant.h"
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
			int ssbo_binds = 0;
			int time_ms = 0;

			void Reset();
		};

		virtual ~RenderPipeline() = default;

		virtual void BeginFrame() = 0;
		virtual void Clear() = 0;
		virtual void Submit(RenderCommandVariant& renderCommand) = 0;
		virtual void EndFrame() = 0;
		virtual void Display() = 0;

		//called by render passes when a new shader is bound
		virtual void OnShaderBound(Shader& shader) = 0;

		//called by render passes when a new texture is bound
		virtual void OnTextureBound(Texture& texture) = 0;

		virtual void SetGlobalUniform(const std::string& name, const glm::mat4& value) = 0;
		virtual void SetGlobalUniform(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetGlobalUniform(const std::string& name, float value) = 0;
		virtual void SetGlobalUniform(const std::string& name, int value) = 0;

		virtual RenderStats& GetRenderStats() = 0;
	};
}