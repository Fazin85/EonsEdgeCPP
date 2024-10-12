#pragma once

#include <array>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>

#include "chunk.h"
#include <map>
#include <unordered_map>
#include <atomic>

#include "chunk_renderer.h"
#include "level.h"
#include "chunk_mesh_data.h"
#include "directions.h"

namespace Eon
{
	class LevelRenderer
	{
	public:
		LevelRenderer();
		~LevelRenderer();
		void SetLevel(Level* level);
		void MeshChunk(Chunk* chunk);
		void RemoveMesh(ChunkPosition chunkPosition);
		void Update();
		std::queue<ChunkPosition>& MeshesToSetup();

	private:
		void BuildChunkMesh(Chunk* chunk);
		void MeshThread();
		void AddFace(ChunkMeshData& meshData, const glm::ivec3& blockPosition, BlockType blockType, Directions direction);
		std::array<u8, 12> GetFaceDataFromDirection(Directions dir);
		const std::map<Directions, std::vector<glm::vec2>> GetUVsFromCoordinates(std::map<const Directions, const glm::vec2>& coords);
		void AddIndices(ChunkMeshData& meshData, int count);

		std::thread mesh_thread;
		std::atomic_bool exit;
		std::mutex chunk_push_mutex;
		std::mutex chunk_pop_mutex;
		std::mutex add_mesh_mutex;
		std::queue<Chunk*> chunks_to_mesh;
		std::queue<ChunkPosition> meshes_to_setup;
		Level* level;
		std::unordered_map<ChunkPosition, std::unique_ptr<ChunkRenderer>> chunk_renderers;
	};
}  // namespace Eon