#include "render_command_mesh.h"
#include "../assert.h"

namespace Eon
{
	RenderCommandMesh::RenderCommandMesh(Mesh& mesh, const glm::mat4& transform, float depth, Material material) :
		mesh(mesh),
		transform(transform),
		depth(depth),
		material(material)
	{
	}

	void RenderCommandMesh::Execute(RenderState& renderState)
	{
		EON_ASSERT(renderState.shader, "No shader bound");

		renderState.shader->UniformMatrix4("model", transform);
		mesh.Render();
	}

	float RenderCommandMesh::GetDepth() const
	{
		return depth;
	}

	Material RenderCommandMesh::GetMaterial() const
	{
		return material;
	}

}