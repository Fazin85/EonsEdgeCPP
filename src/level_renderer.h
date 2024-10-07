#pragma once

#include <array>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>

#include "chunk.h"

#include "chunk_renderer.h"
#include "level.h"
#include "directions.h"

namespace Eon
{
	class LevelRenderer
	{
	public:
		LevelRenderer();
		void SetLevel(Level* level);
		void MeshChunk(Chunk* chunk);

	private:
		void BuildChunkMesh(Chunk* chunk);
		void MeshThread();
		void AddFace(std::vector<glm::ivec3>& vertexData, const glm::ivec3& blockPosition, u8 blockId, Directions direction);
		std::array<u8, 12> GetFaceDataFromDirection(Directions dir);

		std::mutex chunk_push_mutex;
		std::mutex chunk_pop_mutex;
		std::thread mesh_thread;
		std::queue<Chunk*> chunks_to_mesh;
		Level* level;
		std::vector<std::shared_ptr<ChunkRenderer>> chunk_renderers;
	};
}  // namespace Eon