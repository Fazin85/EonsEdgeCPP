#pragma once

#include "render_command.h"
#include "../mesh.h"

namespace Eon
{
	class MeshRenderCommand : public RenderCommand
	{
	public:
		MeshRenderCommand(Mesh& mesh, const glm::mat4& transform, float depth, Material material);

		void Execute(RenderState& renderState) override;
		float GetDepth() const override;
		Material GetMaterial() const override;
	private:
		Mesh& mesh;
		glm::mat4 transform;
		float depth;
		Material material;
	};
}