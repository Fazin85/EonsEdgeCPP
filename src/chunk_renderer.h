#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "num.h"
#include "vertex_array_object.h"
#include "vertex_buffer_object.h"
#include "index_buffer_object.h"
#include "chunk_mesh_data.h"
#include "aabb.h"
#include "chunk.h"

namespace Eon
{
	class ChunkRenderer
	{
	public:
		ChunkRenderer(Chunk* chunk, ChunkMeshData& meshData);
		~ChunkRenderer();
		void SetWaterMesh(ChunkRenderer* waterMesh);
		void Render();
		void Setup();
		void Destroy();
		AABB& GetAABB();

	private:
		bool setup;
		Chunk* chunk;
		std::vector<u32> vertex_position_data;
		std::vector<u32> dir_light_data;
		std::vector<u32> indices;
		ChunkRenderer* water_mesh;
		VertexArrayObject vao;
		IndexBufferObject ibo;
		VertexBufferObject vertex_position_vbo;
		VertexBufferObject dir_light_vbo;
		AABB aabb;
	};
}  // namespace Eon