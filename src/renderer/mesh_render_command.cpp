#include "mesh_render_command.h"
#include "../assert.h"

namespace Eon
{
	MeshRenderCommand::MeshRenderCommand(Mesh& mesh, const glm::mat4& transform, float depth, Material material) :
		mesh(mesh),
		transform(transform),
		depth(depth),
		material(material)
	{
	}

	void MeshRenderCommand::Execute(RenderState& renderState)
	{
		EON_ASSERT(renderState.shader, "No shader bound");

		renderState.shader->UniformMatrix4("model", transform);
		mesh.Render();
	}

	float MeshRenderCommand::GetDepth() const
	{
		return depth;
	}

	Material MeshRenderCommand::GetMaterial() const
	{
		return material;
	}

}