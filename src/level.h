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

#define LEVEL_WIDTH_CHUNKS 32

namespace Eon
{
	class Level
	{
	public:
		Level(AbstractLevelGenerator& abstractLevelGenerator);
		~Level();
		std::optional<std::reference_wrapper<Chunk>> GetChunk(ChunkPosition position);
		std::optional<std::reference_wrapper<Chunk>> GetChunk(ChunkPosition position, bool lock);
		void SetBlock(Block block, int x, int y, int z);
		Block GetBlock(glm::ivec3 position);
		glm::vec4& SkyColor();
		std::vector<BoundingBox> GetBlockAABBs(const BoundingBox& bb);
		bool ChunkExistsAt(ChunkPosition position);
		void Update(ChunkPosition playerChunkPosition, int simulationDistance);
		void AddChunkUnloadedEventListener(ChunkUnloadedEventListener& eventListener);
		std::vector<std::reference_wrapper<Chunk>> GetChunks(ChunkPosition* chunkPositions, int size);

	private:
		void PlaceModel(VoxelModel& model, short x, short y, short z);
		void ChunkGenThread();
		void LoadNewChunks(ChunkPosition& playerChunkPosition, int simulationDistanceBlocks);
		void UnloadFarChunks(ChunkPosition& playerChunkPosition, int unloadDistance);

		std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>> chunks;
		std::vector<ChunkPosition> chunks_being_generated;
		moodycamel::ConcurrentQueue<ChunkPosition> chunks_to_generate;
		moodycamel::ConcurrentQueue<std::unique_ptr<Chunk>> generated_chunks;
		std::thread chunk_gen_thread;
		glm::vec4 sky_color;
		AbstractLevelGenerator& abstract_level_generator;
		std::unique_ptr<VoxelModel> tree_model;
		std::mutex chunk_mutex;
		std::atomic_bool exit;
		std::vector<ChunkUnloadedEventListener*> chunk_unloaded_event_listeners;
	};
} // namespace Eon