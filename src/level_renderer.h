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
#include <set>

namespace Eon
{
	class Level;
	class RenderPipeline;
	class BlockTextureProvider;

	class LevelRenderer : public ChunkUnloadedEventListener
	{
	public:
		LevelRenderer(Level& level, std::unique_ptr<ChunkRendererContainerProvider> chunkRendererContainerProvider);
		void MeshChunk(ChunkPosition chunkPosition);
		void RemoveMesh(ChunkPosition chunkPosition);
		void Update(const Frustum& frustum, glm::dvec3 cameraPosition);
		void Render(RenderPipeline& renderPipeline, RenderCommandPool& commandPool, Camera& camera, glm::dvec3 cameraPosition) const;
		size_t ChunkRendererCount() const;
		bool IsChunkBeingMeshed(ChunkPosition position);
		void OnChunkUnloaded(std::shared_ptr<Chunk> chunk) override;

	private:
		void BuildLODStructure(glm::dvec3 cameraPosition, const Frustum& frustum,
			std::set<ChunkPosition>& requiredChunks);
		void RefineChunk(const ChunkPosition& chunkPos, glm::dvec3 cameraPosition,
			const Frustum& frustum, std::set<ChunkPosition>& requiredChunks);
		bool ShouldRefineChunk(const ChunkPosition& chunkPos, glm::dvec3 cameraPosition);
		void RemoveUnusedChunks(const std::set<ChunkPosition>& requiredChunks);
		void SortRenderersByDistance(std::vector<std::pair<ChunkRendererContainer*, float>>& renderers, glm::dvec3 cameraPosition) const;
		void SortChunksByDistance(std::vector<ChunkPosition>& chunks, glm::dvec3 cameraPosition) const;
		void ProcessSingleChunkMesh();
		bool CanChunkBeMeshed(ChunkPosition position, const Frustum* frustum);
		void MarkCanUnloadForMeshing(ChunkPosition position, bool canUnload);

		// LOD configuration
		static constexpr int MAX_LOD_LEVEL = 8; // Maximum LOD multiplier
		static constexpr double LOD_SWITCH_DISTANCE = CHUNK_WIDTH * 2.5; // Distance to switch LOD

		std::vector<ChunkPosition> chunks_to_mesh;
		std::unique_ptr<ChunkRendererContainerProvider> chunk_renderer_container_provider;
		Level& level;
		std::unordered_map<ChunkPosition, std::unique_ptr<ChunkRendererContainer>> chunk_renderers;
	};
}  // namespace Eon