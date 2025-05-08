#include "level.h"
#include "log.h"
#include "scope_timer.h"
#include <filesystem>
#include <future>
#include <random>
#include <thread>
#include <glm/glm.hpp>

namespace Eon
{
	Level::Level(AbstractLevelGenerator& abstractLevelGenerator) :
		exit(false),
		abstract_level_generator(abstractLevelGenerator),
		chunks{},
		sky_color{},
		chunk_mutex{},
		chunk_gen_thread(&Level::ChunkGenThread, this)
	{
		tree_model = std::make_unique<VoxelModel>("tree.vox");
	}

	Level::~Level()
	{
		exit = true;
	}

	std::optional<std::shared_ptr<Chunk>> Level::GetChunk(ChunkPosition position)
	{
		return GetChunk(position, true);
	}

	std::optional<std::shared_ptr<Chunk>> Level::GetChunk(ChunkPosition position, bool lock)
	{
		if (lock) {
			chunk_mutex.lock();
		}

		std::optional<std::shared_ptr<Chunk>> result;

		if (auto iter = chunks.find(position); iter != chunks.end())
		{
			result = iter->second;
		}

		if (lock) {
			chunk_mutex.unlock();
		}

		return result;
	}

	void Level::SetBlock(Block block, int x, int y, int z)
	{
		std::scoped_lock<std::mutex> lock(chunk_mutex);

		auto chunkPosition = ChunkPosition{ x, z }.Validate();

		auto chunk = GetChunk(chunkPosition, false);

		if (chunk) {
			chunk.value()->SetBlock(x - chunkPosition.x, y, z - chunkPosition.z, block);
		}
	}

	glm::vec4& Level::SkyColor()
	{
		return sky_color;
	}

	std::vector<BoundingBox> Level::GetBlockAABBs(const BoundingBox& bb)
	{
		std::vector<BoundingBox> bbs;

		auto min = glm::ivec3(static_cast<int>(bb.min.x), static_cast<int>(bb.min.y), static_cast<int>(bb.min.z));
		auto max = glm::ivec3(static_cast<int>(bb.max.x) + 1, static_cast<int>(bb.max.y) + 1, static_cast<int>(bb.max.z) + 1);

		for (int x = min.x; x <= max.x; x++)
		{
			for (int y = min.y; y <= max.y; y++)
			{
				for (int z = min.z; z <= max.z; z++)
				{
					Block block = GetBlock({ x, y, z });

					if (block.type != BlockType::AIR)
					{
						bbs.emplace_back(glm::vec3(x, y, z), glm::vec3(x + 1, y + 1, z + 1));
					}
				}
			}
		}

		return bbs;
	}

	bool Level::ChunkExistsAt(ChunkPosition position)
	{
		std::scoped_lock<std::mutex> lock(chunk_mutex);

		return chunks.contains(position);
	}

	void Level::Update(ChunkPosition playerChunkPosition, int simulationDistance)
	{
		playerChunkPosition.Validate();
		int simulationDistanceBlocks = simulationDistance * CHUNK_WIDTH;

		LoadNewChunks(playerChunkPosition, simulationDistanceBlocks);

		int unloadDistance = simulationDistanceBlocks + (CHUNK_WIDTH * 2);
		UnloadFarChunks(playerChunkPosition, unloadDistance);
	}

	void Level::AddChunkUnloadedEventListener(ChunkUnloadedEventListener& eventListener)
	{
		chunk_unloaded_event_listeners.push_back(&eventListener);
	}

	std::vector<std::shared_ptr<Chunk>> Level::GetChunks(std::span<ChunkPosition> chunkPositions)
	{
		std::vector<std::shared_ptr<Chunk>> resultChunks;

		std::scoped_lock<std::mutex> lock(chunk_mutex);

		for (const auto& position : chunkPositions)
		{
			auto chunk = GetChunk(position, false);

			if (chunk)
			{
				resultChunks.push_back(*chunk);
			}
		}

		return resultChunks;
	}

	Block Level::GetBlock(glm::ivec3 position)
	{
		std::scoped_lock<std::mutex> lock(chunk_mutex);

		if (auto chunk = GetChunk(ChunkPosition{ position.x, position.z }.Validate(), false); chunk)
		{
			return chunk.value()->GetBlock(position.x - chunk.value()->Position().x, position.y, position.z - chunk.value()->Position().z);
		}

		return { BlockType::AIR };
	}

	void Level::ChunkGenThread()
	{
		ChunkPosition chunkPosition;
		auto chunkPrimer = std::make_unique<ChunkPrimer>();

		while (!exit) {
			if (chunks_to_generate.try_dequeue(chunkPosition)) {
				abstract_level_generator.GenerateTerrainShape(*chunkPrimer, chunkPosition.x, chunkPosition.z);
				generated_chunks.enqueue(std::make_shared<Chunk>(*chunkPrimer, chunkPosition));
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(16));
			}
		}
	}

	void Level::LoadNewChunks(const ChunkPosition& playerChunkPosition, int simulationDistanceBlocks)
	{
		static std::vector<ChunkPosition> toGen;
		toGen.clear();

		for (int cx = playerChunkPosition.x - simulationDistanceBlocks; cx <= playerChunkPosition.x + simulationDistanceBlocks; cx += CHUNK_WIDTH) {
			for (int cz = playerChunkPosition.z - simulationDistanceBlocks; cz <= playerChunkPosition.z + simulationDistanceBlocks; cz += CHUNK_WIDTH) {
				ChunkPosition currentChunkPosition{ cx, cz };
				currentChunkPosition.Validate();

				if (!ChunkExistsAt(currentChunkPosition) &&
					std::find(chunks_being_generated.begin(), chunks_being_generated.end(), currentChunkPosition) == chunks_being_generated.end()) {
					chunks_being_generated.push_back(currentChunkPosition);
					toGen.push_back(currentChunkPosition);
				}
			}
		}

		//load close chunks first
		std::sort(toGen.begin(), toGen.end(), [&playerChunkPosition](const ChunkPosition& a, const ChunkPosition& b) {
			glm::vec2 pcp(playerChunkPosition.x, playerChunkPosition.z);
			return glm::distance(pcp, glm::vec2(a.x, a.z)) < glm::distance(pcp, glm::vec2(b.x, b.z));
			});

		chunks_to_generate.enqueue_bulk(toGen.data(), toGen.size());

		constexpr size_t maxItems = 32;

		static std::array<std::shared_ptr<Chunk>, maxItems> items;
		size_t actualCount = generated_chunks.try_dequeue_bulk(items.data(), maxItems);

		if (actualCount > 0) {
			std::scoped_lock<std::mutex> lock(chunk_mutex);

			for (int i = 0; i < actualCount; i++) {
				std::shared_ptr<Chunk> chunk = items[i];

				chunks_being_generated.erase(std::find(chunks_being_generated.begin(), chunks_being_generated.end(), chunk->Position()));
				chunks[chunk->Position()] = chunk;
			}
		}
	}

	void Level::UnloadFarChunks(const ChunkPosition& playerChunkPosition, int unloadDistance)
	{
		static std::vector<ChunkPosition> chunksToUnload;
		chunksToUnload.clear();

		std::scoped_lock<std::mutex> lock(chunk_mutex);

		for (const auto& [position, chunk] : chunks) {
			if (chunk->CanUnload()) {
				float distance = glm::distance(glm::vec2(playerChunkPosition.x, playerChunkPosition.z), glm::vec2(position.x, position.z));

				if (distance >= static_cast<float>(unloadDistance)) {
					chunksToUnload.push_back(position);
				}
			}
		}

		for (const ChunkPosition& position : chunksToUnload) {
			auto iter = chunks.find(position);

			for (auto eventListener : chunk_unloaded_event_listeners) {
				eventListener->OnChunkUnloaded(*iter->second);
			}

			chunks.erase(iter);
		}
	}
}  // namespace Eon
