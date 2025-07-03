#pragma once

#include "block_face_texture.h"
#include "camera.h"
#include "chunk.h"
#include "directions.h"
#include "level.h"
#include "chunk_renderer_container_provider.h"
#include "shader.h"
#include "texture_array.h"
#include "chunk_unloaded_event_listener.h"
#include "renderer/render_pipeline.h"
#include "renderer/render_command_pool.h"
#include "ssbo.h"
#include <array>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Eon
{
	class Level;
	class RenderPipeline;
	class BlockTextureProvider;

	class LevelRenderer : public ChunkUnloadedEventListener
	{
	public:
		LevelRenderer(Level& level, std::unique_ptr<ChunkRendererContainerProvider> chunkRendererContainerProvider);
		~LevelRenderer() final;
		void MeshChunk(ChunkPosition chunkPosition);
		void RemoveMesh(ChunkPosition chunkPosition);
		void Update(const Frustum& frustum, glm::vec3 cameraPosition);
		void Render(RenderPipeline& renderPipeline, RenderCommandPool& commandPool, Camera& camera, ChunkPosition playerChunkPosition);
		size_t ChunkRendererCount();
		bool IsChunkBeingMeshed(ChunkPosition position);
		void OnChunkUnloaded(std::shared_ptr<Chunk> chunk) override;

	private:
		void SortRenderersByDistance(std::vector<std::pair<ChunkRendererContainer*, float>>& renderers, glm::vec3 cameraPosition) const;
		void SortChunksByDistance(std::vector<ChunkPosition>& chunks, glm::vec3 cameraPosition) const;
		void ProcessSingleChunkMesh();
		bool CanChunkBeMeshed(ChunkPosition position, const Frustum* frustum);
		void MarkCanUnloadForMeshing(ChunkPosition position, bool canUnload);

		std::vector<ChunkPosition> chunks_to_mesh;
		std::unique_ptr<ChunkRendererContainerProvider> chunk_renderer_container_provider;
		Level& level;
		SSBO ssbo;
		std::unordered_map<ChunkPosition, std::unique_ptr<ChunkRendererContainer>> chunk_renderers;
	};
}  // namespace Eon