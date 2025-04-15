#pragma once

#include "block_face_texture.h"
#include "camera.h"
#include "chunk.h"
#include "chunk_renderer.h"
#include "directions.h"
#include "level.h"
#include "aabb_chunk_renderer.h"
#include "shader.h"
#include "texture_array.h"
#include "chunk_unloaded_event_listener.h"
#include <array>
#include <atomic>
#include <concurrentqueue/concurrentqueue.h>
#include <map>
#include <memory>
#include <unordered_map>
#include <thread>

namespace Eon
{
	class Level;

	class LevelRenderer : public ChunkUnloadedEventListener
	{
	public:
		LevelRenderer();
		~LevelRenderer();
		void SetLevel(Level* level);
		void MeshChunk(ChunkPosition chunkPosition);
		void RemoveMesh(ChunkPosition chunkPosition);
		void Update(glm::vec3 cameraPosition);
		void Render(Camera& camera, glm::vec3 cameraPosition);
		int ChunkRendererCount();
		bool IsChunkBeingMeshed(ChunkPosition position);

		void OnChunkUnloaded(Chunk& chunk) override;

	private:
		void BuildChunkMesh(ChunkPosition inChunkPosition);
		void MeshThread();
		void AddFace(ChunkMeshConstructionData& meshData, const glm::ivec3& blockPosition, BlockType blockType, Directions direction);
		std::array<unsigned char, 12> GetFaceDataFromDirection(Directions dir);
		void AddIndices(ChunkMeshConstructionData& meshData, int count);
		unsigned int GetLod(float distance);
		BlockFaceTexture GetTextureId(BlockType blockType, Directions faceDirection);
		bool CanChunkBeMeshed(ChunkPosition position);
		void MarkCanUnloadForMeshing(ChunkPosition position, bool canUnload);

		std::unique_ptr<Shader> chunk_shader;
		std::unique_ptr<TextureArray> chunk_texture;
		std::vector<std::thread> mesh_threads;
		std::vector<ChunkPosition> frozen_chunks;
		std::vector<ChunkPosition> chunks_to_mesh_vector;
		std::atomic_bool exit;
		moodycamel::ConcurrentQueue<ChunkPosition> chunks_to_mesh;
		moodycamel::ConcurrentQueue<std::unique_ptr<AABBChunkRenderer>> meshes_to_setup;
		Level* level;
		std::unordered_map<ChunkPosition, std::unique_ptr<AABBChunkRenderer>> chunk_renderers;
	};
}  // namespace Eon