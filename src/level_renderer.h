#pragma once

#include <array>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "chunk.h"
#include <atomic>
#include <concurrentqueue/concurrentqueue.h>
#include <map>
#include <parallel_hashmap/phmap.h>

#include "chunk_mesh_data.h"
#include "chunk_renderer.h"
#include "directions.h"
#include "level.h"
#include "shader.h"
#include "texture.h"

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
		void Render();

	private:
		void BuildChunkMesh(Chunk* chunk);
		void MeshThread();
		void AddFace(ChunkMeshData& meshData, const glm::ivec3& blockPosition, BlockType blockType, Directions direction);
		std::array<u8, 12> GetFaceDataFromDirection(Directions dir);
		const std::map<Directions, std::vector<glm::vec2>> GetUVsFromCoordinates(std::map<const Directions, const glm::vec2>& coords);
		void AddIndices(ChunkMeshData& meshData, int count);

		std::unique_ptr<Shader> chunk_shader;
		std::unique_ptr<Texture> chunk_texture;
		std::thread mesh_thread;
		std::atomic_bool exit;
		moodycamel::ConcurrentQueue<Chunk*> chunks_to_mesh;
		moodycamel::ConcurrentQueue<ChunkPosition> meshes_to_setup;
		Level* level;
		phmap::parallel_node_hash_map<ChunkPosition, std::unique_ptr<ChunkRenderer>> chunk_renderers;
	};
}  // namespace Eon