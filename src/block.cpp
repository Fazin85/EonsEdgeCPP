#include "block.h"

namespace Eon
{
	Block::Block(uint8_t id, BlockType type, std::optional<std::function<std::unique_ptr<BlockEntity>()>> createBlockEntity, bool isCutout, bool translucent) : id(id), type(type), create_block_entity(std::nullopt), is_cutout(isCutout), translucent(translucent)
	{
		create_block_entity = createBlockEntity;
	}

	bool Block::operator==(const Block& other) const
	{
		return (id == other.id);
	}

	uint8_t Block::GetID() const
	{
		return id;
	}

	BlockType Block::GetType() const
	{
		return type;
	}

	bool Block::IsBlockEntity() const
	{
		return create_block_entity.has_value();
	}

	std::unique_ptr<BlockEntity> Block::CreateBlockEntityInstance()
	{
		const auto& func = create_block_entity.value();
		return func();
	}

	bool Block::IsCutout() const
	{
		return is_cutout;
	}

	bool Block::Translucent() const
	{
		return translucent;
	}

	BlockBuilder::BlockBuilder(BlockType type, uint8_t& i) : id(i++), type(type), is_cutout(false), translucent(false)
	{
	}

	BlockBuilder& BlockBuilder::SetType(BlockType t)
	{
		type = t;
		return *this;
	}

	BlockBuilder& BlockBuilder::SetBlockEntity(std::function<std::unique_ptr<BlockEntity>()> createBlockEntity)
	{
		create_block_entity = createBlockEntity;
		return *this;
	}

	BlockBuilder& BlockBuilder::SetIsCutout()
	{
		is_cutout = true;
		return *this;
	}

	BlockBuilder& BlockBuilder::SetTranslucent()
	{
		translucent = true;
		return *this;
	}

	Block BlockBuilder::Build() const
	{
		return Block(id, type, create_block_entity, is_cutout, translucent);
	}

	BlockRegistry::BlockRegistry() : blocks()
	{
		using enum Eon::BlockType;

		uint8_t id = 0;
		RegisterBlock(BlockBuilder(AIR, id).SetIsCutout().Build());
		RegisterBlock(BlockBuilder(STONE, id).Build());
		RegisterBlock(BlockBuilder(GRASS, id).Build());
		RegisterBlock(BlockBuilder(DIRT, id).Build());
		RegisterBlock(BlockBuilder(WATER, id).SetTranslucent().Build());
		RegisterBlock(BlockBuilder(SAND, id).Build());
		RegisterBlock(BlockBuilder(OAK_LOG, id).Build());
		RegisterBlock(BlockBuilder(LEAF, id).SetIsCutout().Build());
		RegisterBlock(BlockBuilder(GRAVEL, id).Build());
	}

	BlockRegistry& BlockRegistry::GetInstance()
	{
		static BlockRegistry instance;
		return instance;
	}

	const Block& BlockRegistry::GetBlockByType(BlockType type)
	{
		return *GetInstance().block_map.at(type);
	}

	const Block& BlockRegistry::GetBlockByID(uint8_t id)
	{
		return *GetInstance().blocks[id].get();
	}

	void BlockRegistry::RegisterBlock(Block block)
	{
		auto ptr = std::make_shared<Block>(block);
		block_map[block.GetType()] = ptr;
		blocks[block.GetID()] = ptr;
	}
}