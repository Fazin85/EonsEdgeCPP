#include "mesh_render_command_colored.h"

namespace Eon
{
	MeshRenderCommandColored::MeshRenderCommandColored(
		Mesh& mesh,
		const glm::mat4& transform,
		float depth,
		Material material,
		float r,
		float g,
		float b) : MeshRenderCommand(mesh, transform, depth, material), r(r), g(g), b(b)
	{
	}

	void MeshRenderCommandColored::Execute(RenderState& renderState)
	{
		renderState.GetShader()->UniformFVec3("objectColor", glm::vec3(r, g, b));
		MeshRenderCommand::Execute(renderState);
	}
}