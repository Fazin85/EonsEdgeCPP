#include "default_render_pipeline.h"
#include "opaque_render_pass.h"
#include "translucent_render_pass.h"
#include "ssr_render_pass.h"
#include "../../framebuffer.h"

namespace Eon
{
	DefaultRenderPipeline::DefaultRenderPipeline()
	{
		Framebuffer::FramebufferSpec spec;
		spec.width = Window::Get().getSize().x;
		spec.height = Window::Get().getSize().y;

		spec.colorAttachments = {
			// Albedo
			{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
			// Normal 
			{ GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
			// Position + Depth
			{ GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
		};

		// Add depth buffer
		spec.hasDepthAttachment = true;

		g_buffer = std::make_unique<Framebuffer>(spec);

		//Framebuffer::FramebufferSpec ssrSpec;
		//ssrSpec.width = Window::Get().getSize().x;
		//ssrSpec.height = Window::Get().getSize().y;

		//ssrSpec.colorAttachments = {
		//	// Albedo
		//	{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
		//};

		//ssr_buffer = std::make_unique<Framebuffer>(ssrSpec);

		render_passes.emplace_back(std::make_unique<OpaqueRenderPass>(*this, *g_buffer));
		render_passes.emplace_back(std::make_unique<TranslucentRenderPass>(*this, *g_buffer));
		//render_passes.emplace_back(std::make_unique<SSRRenderPass>(*this, *ssr_buffer));
	}

	void DefaultRenderPipeline::BeginFrame()
	{
		render_stats.Reset();
	}

	void DefaultRenderPipeline::Clear()
	{
		render_state.SetFramebuffer(g_buffer.get());
		render_state.Apply();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

	void DefaultRenderPipeline::Display()
	{
		g_buffer->BlitToScreen();
		//ssr_buffer->BlitToScreen();
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
