#include "render_pass_ssr.h"

namespace Eon
{
	RenderPassSSR::RenderPassSSR(RenderPipeline& pipeline) : RenderPass(pipeline, "StandardSSR")
	{
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> positions = {
				{
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(1.0f, 0.0f, 0.0f),
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

		fullscreen_quad = std::make_unique<PositionTextureMesh>(std::move(positions), std::move(uvs));
		fullscreen_quad->Setup();
	}

	void RenderPassSSR::Execute(RenderState& renderState)
	{
		throw new std::exception("NOP");
	}

	void RenderPassSSR::End(RenderState& renderState)
	{
		
		
		RenderPass::End(renderState);
	}
}
