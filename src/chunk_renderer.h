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
		Chunk& GetChunk();

	private:
		bool setup;
		Chunk* chunk;
		u32* vertex_position_data;
		u32* dir_light_data;
		u32* indices;
		size_t vertex_size;
		size_t index_size;
		ChunkRenderer* water_mesh;
		VertexArrayObject vao;
		IndexBufferObject ibo;
		VertexBufferObject vertex_position_vbo;
		VertexBufferObject dir_light_vbo;
		AABB aabb;
	};
}  // namespace Eon