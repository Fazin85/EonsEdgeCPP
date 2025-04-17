#pragma once

#include "block_face_texture.h"
#include "camera.h"
#include "chunk.h"
#include "chunk_renderer.h"
#include "directions.h"
#include "level.h"
#include "chunk_renderer_container_provider.h"
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
		LevelRenderer(Level& level, ChunkRendererContainerProvider& chunkRendererContainerProvider);
		~LevelRenderer();
		void MeshChunk(ChunkPosition chunkPosition);
		void RemoveMesh(ChunkPosition chunkPosition);
		void Update(Frustum& frustum, glm::vec3 cameraPosition);
		void Render(Camera& camera, glm::vec3 cameraPosition);
		int ChunkRendererCount();
		bool IsChunkBeingMeshed(ChunkPosition position);

		void OnChunkUnloaded(Chunk& chunk) override;

	private:
		void SortRenderersByDistance(std::vector<std::pair<ChunkRendererContainer*, float>>& renderers, glm::vec3 cameraPosition);
		void MeshThread();
		bool CanChunkBeMeshed(ChunkPosition position, Frustum& frustum);
		void MarkCanUnloadForMeshing(ChunkPosition position, bool canUnload);

		std::unique_ptr<Shader> chunk_shader;
		std::unique_ptr<TextureArray> chunk_texture;
		std::vector<std::thread> mesh_threads;
		std::vector<ChunkPosition> chunks_to_mesh_vector;
		std::atomic_bool exit;
		ChunkRendererContainerProvider& chunk_renderer_container_provider;
		moodycamel::ConcurrentQueue<ChunkPosition> chunks_to_mesh;
		moodycamel::ConcurrentQueue<std::unique_ptr<ChunkRendererContainer>> meshes_to_setup;
		Level& level;
		std::unordered_map<ChunkPosition, std::unique_ptr<ChunkRendererContainer>> chunk_renderers;
	};
}  // namespace Eon