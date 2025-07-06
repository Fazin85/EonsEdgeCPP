#include <glm/gtc/matrix_transform.hpp>
#include "ssr_render_pass.h"
#include "../../framebuffer.h"

namespace Eon
{
	SSRRenderPass::SSRRenderPass(RenderPipeline& pipeline, Framebuffer& framebuffer) : FrameBufferRenderPass(pipeline, "StandardSSR", framebuffer)
	{
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> positions = {
				{
				glm::vec3(-1.0f, 1.0f, 0.0f),
				glm::vec3(-1.0f, -1.0f, 0.0f),
				glm::vec3(1.0f, -1.0f, 0.0f),
				glm::vec3(-1.0f, 1.0f, 0.0f),
				glm::vec3(1.0f, -1.0f, 0.0f),
				glm::vec3(1.0f, 1.0f, 0.0f)
				},
				*PoolAllocators::GetInstance().vec3_allocator
		};

		std::vector<glm::vec2, PoolAllocator<glm::vec2>> uvs = {
			{
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f)
			},
			*PoolAllocators::GetInstance().vec2_allocator
		};

		ssr_shader = AssetManager::GetAsset<Shader>("shader.ssr").GetID();

		fullscreen_quad = std::make_unique<PositionTextureMesh>(std::move(positions), std::move(uvs));
		fullscreen_quad->Setup();
	}

	void SSRRenderPass::Execute(RenderState& renderState)
	{
	}

	void SSRRenderPass::End(RenderState& renderState)
	{
		Framebuffer& framebuffer = GetFramebuffer();
		renderState.SetFramebuffer(&framebuffer);
		renderState.SetShader(ssr_shader);
		renderState.Apply();
		renderState.BindGBufferTextures();

		auto idt = glm::identity<glm::mat4>();

		auto shader = ssr_shader.Get<Shader>();
		pipeline.OnShaderBound(*shader);
		shader->UniformMatrix4("projection", idt);
		shader->UniformMatrix4("view", idt);
		shader->UniformMatrix4("model", idt);
		shader->UniformMatrix4("invView", glm::inverse(idt));
		shader->UniformMatrix4("invProjection", glm::inverse(idt));

		shader->UniformInt1("gAlbedo", 0);
		shader->UniformInt1("gNormal", 1);
		shader->UniformInt1("gPosition", 2);

		shader->UniformFloat("maxDistance", 100.0f);
		shader->UniformInt1("maxSteps", 128);
		shader->UniformFloat("thickness", 0.25f);
		shader->UniformFloat("fadeDistance", 75.0f);
		shader->UniformFloat("edgeFadeDistance", 0.1f);

		fullscreen_quad->Render();

		renderState.SetSSRTexture(framebuffer.GetColorAttachment(0));

		RenderPass::End(renderState);
	}

	void SSRRenderPass::Submit(RenderCommandVariant& renderCommand)
	{
		throw new std::exception("NOP");
	}
}
