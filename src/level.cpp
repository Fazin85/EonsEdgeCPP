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

		chunk_gen_thread.join();
	}

	std::optional<std::reference_wrapper<Chunk>> Level::GetChunk(ChunkPosition position)
	{
		return GetChunk(position, true);
	}

	std::optional<std::reference_wrapper<Chunk>> Level::GetChunk(ChunkPosition position, bool lock)
	{
		if (lock) {
			chunk_mutex.lock();
		}

		std::optional<std::reference_wrapper<Chunk>> result;

		const auto iter = chunks.find(position);
		if (iter != chunks.end()) {
			result = *iter->second;
		}

		if (lock) {
			chunk_mutex.unlock();
		}

		return result;
	}

	void Level::SetBlock(Block block, int x, int y, int z)
	{
		std::lock_guard<std::mutex> lock(chunk_mutex);

		auto chunkPosition = ChunkPosition(x, z).Validate();

		auto chunk = GetChunk(chunkPosition, false);

		if (chunk.has_value()) {
			chunk->get().SetBlock(x - chunkPosition.x, y, z - chunkPosition.z, block);
		}
	}

	glm::vec4& Level::SkyColor()
	{
		return sky_color;
	}

	std::vector<BoundingBox> Level::GetBlockAABBs(const BoundingBox& bb)
	{
		std::vector<BoundingBox> bbs;

		glm::ivec3 min = glm::ivec3(static_cast<int>(bb.min.x), static_cast<int>(bb.min.y), static_cast<int>(bb.min.z));
		glm::ivec3 max = glm::ivec3(static_cast<int>(bb.max.x) + 1, static_cast<int>(bb.max.y) + 1, static_cast<int>(bb.max.z) + 1);

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

	short Level::GetHighestBlockY(int x, int z)
	{
		std::lock_guard<std::mutex> lock(chunk_mutex);

		auto chunk = GetChunk(ChunkPosition(x, z).Validate(), false);

		if (!chunk.has_value())
		{
			return 0;
		}

		return chunk->get().GetHeightestBlockY(x - chunk->get().Position().x, z - chunk->get().Position().z);
	}

	bool Level::ChunkExistsAt(ChunkPosition position)
	{
		std::lock_guard<std::mutex> lock(chunk_mutex);

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

	Block Level::GetBlock(glm::ivec3 position)
	{
		std::lock_guard<std::mutex> lock(chunk_mutex);

		auto chunk = GetChunk(ChunkPosition{ position.x, position.z }.Validate(), false);
		if (chunk.has_value())
		{
			return chunk->get().GetBlock(position.x - chunk->get().Position().x, position.y, position.z - chunk->get().Position().z);
		}

		return { BlockType::AIR };
	}

	void Level::PlaceModel(VoxelModel& model, short x, short y, short z)
	{
		for (int mx = 0; mx < model.Size().x; mx++)
		{
			for (int my = 0; my < model.Size().y; my++)
			{
				for (int mz = 0; mz < model.Size().z; mz++)
				{
					BlockType type = model.GetBlockType(mx, my, mz);

					if (type != BlockType::AIR)
					{
						SetBlock(Block(type), x + mx, y + my, z + mz);
					}
				}
			}
		}
	}

	void Level::PlaceTree(short x, short z)
	{
		auto chunk = GetChunk(ChunkPosition(x, z).Validate());

		if (!chunk.has_value())
		{
			return;
		}

		short y = chunk->get().GetHeightestBlockY(x - (chunk->get().Position().x), z - (chunk->get().Position().z));

		for (int yp = y; yp < y + 6; yp++)
		{
			SetBlock({ BlockType::OAKLOG }, x, yp, z);
		}

		for (int px = x - 3; px < x + 3; px++)
		{
			for (int pz = z - 3; pz < z + 3; pz++)
			{
				SetBlock(Block(Eon::BlockType::LEAF), px, y + 5, pz);
			}
		}
	}

	void Level::ChunkGenThread()
	{
		while (!exit) {
			ChunkPosition chunkPosition;
			if (chunks_to_generate.try_dequeue(chunkPosition)) {
				auto chunk = abstract_level_generator.GenerateTerrainShape(chunkPosition.x, chunkPosition.z);
				generated_chunks.enqueue(std::move(chunk));
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(4));
		}
	}

	void Level::LoadNewChunks(ChunkPosition& playerChunkPosition, int simulationDistanceBlocks)
	{
		static std::vector<ChunkPosition> toGen;
		toGen.clear();

		for (int cx = playerChunkPosition.x - simulationDistanceBlocks; cx <= playerChunkPosition.x + simulationDistanceBlocks; cx += CHUNK_WIDTH) {
			for (int cz = playerChunkPosition.z - simulationDistanceBlocks; cz <= playerChunkPosition.z + simulationDistanceBlocks; cz += CHUNK_WIDTH) {
				ChunkPosition currentChunkPosition(cx, cz);
				currentChunkPosition.Validate();

				sf::Clock clock;

				if (!ChunkExistsAt(currentChunkPosition)) {
					if (std::find(chunks_being_generated.begin(), chunks_being_generated.end(), currentChunkPosition) == chunks_being_generated.end()) {
						chunks_being_generated.push_back(currentChunkPosition);
						toGen.push_back(currentChunkPosition);
					}
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

		static std::array<std::unique_ptr<Chunk>, maxItems> items;
		size_t actualCount = generated_chunks.try_dequeue_bulk(items.data(), maxItems);

		if (actualCount > 0) {
			std::lock_guard<std::mutex> lock(chunk_mutex);

			for (int i = 0; i < actualCount; i++) {
				std::unique_ptr<Chunk> chunk = std::move(items[i]);

				chunks_being_generated.erase(std::find(chunks_being_generated.begin(), chunks_being_generated.end(), chunk->Position()));
				chunks[chunk->Position()] = std::move(chunk);
			}
		}
	}

	void Level::UnloadFarChunks(ChunkPosition& playerChunkPosition, int unloadDistance)
	{
		static std::vector<ChunkPosition> chunksToUnload;
		chunksToUnload.clear();

		std::lock_guard<std::mutex> lock(chunk_mutex);

		for (auto& chunk : chunks) {
			if (chunk.second->CanUnload()) {
				float distance = glm::distance(glm::vec2(playerChunkPosition.x, playerChunkPosition.z), glm::vec2(chunk.first.x, chunk.first.z));

				if (distance >= unloadDistance) {
					chunksToUnload.push_back(chunk.first);
				}
			}
		}

		for (ChunkPosition& position : chunksToUnload) {
			auto iter = chunks.find(position);

			for (auto eventListener : chunk_unloaded_event_listeners) {
				eventListener->OnChunkUnloaded(*iter->second);
			}

			chunks.erase(iter);
		}
	}
}  // namespace Eon
