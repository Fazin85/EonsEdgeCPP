#pragma once

#include "mesh_render_command.h"

namespace Eon
{
	class SSBO;

	class ChunkMeshRenderCommand : public MeshRenderCommand
	{
	public:
		ChunkMeshRenderCommand(
			Mesh& mesh, 
			const glm::mat4& transform, 
			float depth, 
			Material material, 
			SSBO& materialBuffer, 
			TextureID blockIDTexture);

		void Execute(RenderState& renderState) override;
	
	private:
		SSBO& ssbo;
		TextureID block_id_texture;
	};
}