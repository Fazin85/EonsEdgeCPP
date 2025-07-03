#include "chunk_mesh_render_command.h"
#include "../../ssbo.h"

namespace Eon
{
	ChunkMeshRenderCommand::ChunkMeshRenderCommand(
		Mesh& mesh,
		const glm::mat4& transform,
		float depth,
		Material material,
		SSBO& materialBuffer,
		TextureID blockIDTexture) : MeshRenderCommand(mesh, transform, depth, material), ssbo(materialBuffer), block_id_texture(blockIDTexture)
	{
	}

	void ChunkMeshRenderCommand::Execute(RenderState& renderState)
	{
		renderState.SetTexture(block_id_texture, 1);
		renderState.SetSSBO(ssbo.GetId(), 0);
		renderState.Apply();

		MeshRenderCommand::Execute(renderState);
	}
}