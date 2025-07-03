#pragma once

#include "mesh_render_command.h"

namespace Eon
{
	class MeshRenderCommandColored : public MeshRenderCommand
	{
	public:
		MeshRenderCommandColored(
			Mesh& mesh, 
			const glm::mat4& transform, 
			float depth, 
			Material material, 
			float r, 
			float g, 
			float b);

		void Execute(RenderState& renderState) override;

	private:
		float r;
		float g;
		float b;
	};
}
