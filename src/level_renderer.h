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
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Eon
{
	class Level;
	class LevelRenderer : public ChunkUnloadedEventListener
	{
	public:
		LevelRenderer(Level& level, std::unique_ptr<ChunkRendererContainerProvider> chunkRendererContainerProvider);
		~LevelRenderer() final;
		void MeshChunk(ChunkPosition chunkPosition);
		void RemoveMesh(ChunkPosition chunkPosition);
		void Update(const Frustum& frustum, glm::vec3 cameraPosition);
		void Render(Camera& camera, glm::vec3 cameraPosition);
		size_t ChunkRendererCount();
		bool IsChunkBeingMeshed(ChunkPosition position);
		void OnChunkUnloaded(std::shared_ptr<Chunk> chunk) override;

	private:
		void SortRenderersByDistance(std::vector<std::pair<ChunkRendererContainer*, float>>& renderers, glm::vec3 cameraPosition) const;
		void SortChunksByDistance(std::vector<ChunkPosition>& chunks, glm::vec3 cameraPosition) const;
		void ProcessSingleChunkMesh();
		bool CanChunkBeMeshed(ChunkPosition position, const Frustum* frustum);
		void MarkCanUnloadForMeshing(ChunkPosition position, bool canUnload);

		std::unique_ptr<Shader> chunk_shader;
		std::unique_ptr<TextureArray> chunk_texture;
		std::vector<ChunkPosition> chunks_to_mesh;
		std::unique_ptr<ChunkRendererContainerProvider> chunk_renderer_container_provider;
		Level& level;
		std::unordered_map<ChunkPosition, std::unique_ptr<ChunkRendererContainer>> chunk_renderers;
	};
}  // namespace Eon