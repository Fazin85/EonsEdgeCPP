#pragma once

#include "block_entity.h"

#include <cstdint>
#include <unordered_map>
#include <array>
#include <optional>
#include <memory>
#include <functional>

namespace Eon
{
	class BlockBuilder;
	class BlockRegistry;
	enum class BlockType;

	class Block
	{
	public:
		Block(uint8_t id, BlockType type, const std::optional<std::function<std::unique_ptr<BlockEntity>(glm::ivec3)>>& createBlockEntity, bool isCutout, bool translucent);

		bool operator==(const Block& other) const;

		uint8_t GetID() const;
		BlockType GetType() const;
		bool IsBlockEntity() const;
		std::unique_ptr<BlockEntity> CreateBlockEntityInstance(glm::ivec3 worldPosition);
		bool IsCutout() const;
		bool Translucent() const;

	private:
		uint8_t id;
		BlockType type;
		std::optional<std::function<std::unique_ptr<BlockEntity>(glm::ivec3)>> create_block_entity;
		bool is_cutout;
		bool translucent;
	};

	class BlockBuilder
	{
	public:
		explicit BlockBuilder(BlockType type, uint8_t& id);
		BlockBuilder& SetType(BlockType type);
		BlockBuilder& SetBlockEntity(const std::function<std::unique_ptr<BlockEntity>(glm::ivec3)>& createBlockEntity);
		BlockBuilder& SetIsCutout();
		BlockBuilder& SetTranslucent();
		Block Build() const;

	private:
		uint8_t id;
		BlockType type;
		std::optional<std::function<std::unique_ptr<BlockEntity>(glm::ivec3)>> create_block_entity;
		bool is_cutout;
		bool translucent;
	};

	enum class BlockType
	{
		AIR, STONE, GRASS, DIRT, WATER, SAND, OAK_LOG, LEAF, GRAVEL
	};

	class BlockRegistry
	{
	public:
		static BlockRegistry& GetInstance();

		static const Block& GetBlockByType(BlockType type);
		static const Block& GetBlockByID(uint8_t id);

	private:
		BlockRegistry();

		void RegisterBlock(Block block);

		std::unordered_map<BlockType, std::shared_ptr<Block>> block_map;
		std::array<std::shared_ptr<Block>, 256> blocks;
	};

}  // namespace Eon