#include "opaque_render_pass.h"

namespace Eon
{
	OpaqueRenderPass::OpaqueRenderPass() : BatchedRenderPass("StandardOpaque") {}

	void OpaqueRenderPass::Begin(RenderState& renderState)
	{
	}

	void OpaqueRenderPass::Execute(RenderState& renderState, Camera& camera, glm::vec3 cameraPosition)
	{
		renderState.Apply();

		BatchedRenderPass::Execute(renderState, camera, cameraPosition);
	}
}