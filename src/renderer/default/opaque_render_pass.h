#pragma once

#include "../batched_render_pass.h"

namespace Eon
{
	class OpaqueRenderPass : public BatchedRenderPass
	{
	public:
		OpaqueRenderPass();

		void Begin(RenderState& renderState) override;
		void Execute(RenderState& renderState, Camera& camera, glm::vec3 cameraPosition) override;
	};
}