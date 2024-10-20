#pragma once

#include "camera.h"
#include "chunk.h"
#include "chunk_mesh_data.h"
#include "chunk_renderer.h"
#include "directions.h"
#include "level.h"
#include "lod_chunk_renderer.h"
#include "shader.h"
#include "texture_array.h"
#include <array>
#include <atomic>
#include <concurrentqueue/concurrentqueue.h>
#include <map>
#include <memory>
#include <parallel_hashmap/phmap.h>
#include <thread>

namespace Eon
{
	class LevelRenderer
	{
	public:
		LevelRenderer();
		~LevelRenderer();
		void SetLevel(Level* level);
		void MeshChunk(ChunkPosition chunkPosition);
		void MeshAllChunks();
		bool MeshingAllChunks() const;
		void RemoveMesh(ChunkPosition chunkPosition);
		void Update(glm::vec3 cameraPosition);
		void Render(Camera& camera, glm::vec3 cameraPosition);
		int ChunkRendererCount();

	private:
		void BuildChunkMesh(ChunkPosition inChunkPosition);
		void MeshThread();
		void AddFace(ChunkMeshData& meshData, const glm::ivec3& blockPosition, BlockType blockType, Directions direction, unsigned int lod);
		std::array<u8, 12> GetFaceDataFromDirection(Directions dir);
		void AddIndices(ChunkMeshData& meshData, int count);
		unsigned int GetLod(float distance);

		std::atomic_bool meshing_all_chunks;
		std::atomic_int meshing_all_meshed_chunks_count;
		int meshing_all_chunks_time_sum = 0;
		std::unique_ptr<Shader> chunk_shader;
		std::unique_ptr<TextureArray> chunk_texture;
		std::vector<std::thread> mesh_threads;
		std::atomic_bool exit;
		moodycamel::ConcurrentQueue<ChunkPosition> chunks_to_mesh;
		moodycamel::ConcurrentQueue<ChunkPosition> meshes_to_setup;
		Level* level;
		phmap::parallel_node_hash_map<ChunkPosition, std::unique_ptr<LODChunkRenderer>> chunk_renderers;
	};
}  // namespace Eon