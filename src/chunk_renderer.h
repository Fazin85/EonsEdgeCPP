#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "chunk_mesh_data.h"
#include "num.h"

namespace Eon
{
	class ChunkRenderer
	{
	public:
		ChunkRenderer(ChunkMeshData& meshData);
		~ChunkRenderer();
		void SetWaterMesh(ChunkRenderer* waterMesh);
		void Render();
		void Setup();
		void Destroy();

	private:
		bool setup;
		u32* vertex_position_data;
		u32* dir_light_data;
		u32* indices;
		size_t vertex_data_size;
		size_t index_size;
		ChunkRenderer* water_mesh;
		//VertexArrayObject vao;
		unsigned int vao;
		//IndexBufferObject ibo;
		unsigned int ibo;
		//VertexBufferObject vertex_position_vbo;
		unsigned int vertex_position_vbo;
		//VertexBufferObject dir_light_vbo;
		unsigned int dir_light_vbo;
	};
}  // namespace Eon