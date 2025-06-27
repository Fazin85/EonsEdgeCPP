#pragma once

#include "block.h"
#include "bounding_box.h"
#include "chunk.h"
#include "chunk_position.h"
#include "voxel_model.h"
#include "abstract_level_generator.h"
#include "chunk_unloaded_event_listener.h"
#include "frustum.h"

#include <array>
#include <fstream>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <unordered_map>
#include <optional>
#include <concurrentqueue/concurrentqueue.h>
#include <span>

namespace Eon
{
	class Level
	{
	public:
		explicit Level(std::unique_ptr<AbstractLevelGenerator> abstractLevelGenerator);
		~Level();
		std::shared_ptr<Chunk> GetChunk(ChunkPosition position);
		void SetBlock(const Block& block, int x, int y, int z);
		const Block& GetBlock(glm::ivec3 position);
		glm::vec4& SkyColor();
		std::vector<BoundingBox> GetBlockAABBs(const BoundingBox& bb);
		bool ChunkExistsAt(ChunkPosition position);
		void Update(ChunkPosition playerChunkPosition, int simulationDistance);
		void AddChunkUnloadedEventListener(ChunkUnloadedEventListener& eventListener);
		std::vector<std::shared_ptr<Chunk>> GetChunks(std::span<ChunkPosition> chunkPositions);

	private:
		void ChunkGenThread();
		void LoadNewChunks(const ChunkPosition& playerChunkPosition, int simulationDistanceBlocks);
		void UnloadFarChunks(const ChunkPosition& playerChunkPosition, int unloadDistance);
		std::optional<std::shared_ptr<Chunk>> GetChunkNoLock(ChunkPosition position);

		std::unordered_map<ChunkPosition, std::shared_ptr<Chunk>> chunks;
		std::vector<ChunkPosition> chunks_being_generated;
		moodycamel::ConcurrentQueue<ChunkPosition> chunks_to_generate;
		moodycamel::ConcurrentQueue<std::shared_ptr<Chunk>> generated_chunks;
		glm::vec4 sky_color;
		std::unique_ptr<AbstractLevelGenerator> abstract_level_generator;
		std::jthread chunk_gen_thread;
		std::unique_ptr<VoxelModel> tree_model;
		std::mutex chunk_mutex;
		std::atomic_bool exit;
		std::vector<ChunkUnloadedEventListener*> chunk_unloaded_event_listeners;
	};
} // namespace Eon