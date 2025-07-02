#pragma once

#include "block_entity.h"

#include <cstdint>
#include <unordered_map>
#include <array>
#include <optional>
#include <memory>
#include <functional>
#include "chunk_mesh_construction_data.h"
#include "directions.h"
#include "asset_manager.h"

namespace Eon
{
	class BlockBuilder;
	class BlockRegistry;
	class Block;
	class BlockTextureProvider;

	struct BlockRenderContext
	{
		const glm::ivec3 blockPosition;
		const Block& centerBlock;
		const Block& topBlock;
		const Block& bottomBlock;
		const Block& leftBlock;
		const Block& rightBlock;
		const Block& frontBlock;
		const Block& backBlock;
		ChunkMeshConstructionData& meshData;
		const BlockTextureProvider& uvProvider;
	};

	std::array<unsigned char, 12> GetFaceDataFromDirection(Directions dir);
	void AddFace(BlockRenderContext& renderContext, Directions direction);

	class Block
	{
	public:
		Block(
			uint8_t id,
			const std::string& type,
			const std::optional<std::function<std::unique_ptr<BlockEntity>(glm::ivec3)>>& createBlockEntity,
			bool isCutout,
			bool translucent,
			std::function<void(
				BlockRenderContext&)>& render);

		bool operator==(const Block& other) const;

		uint8_t GetID() const;
		const std::string& GetType() const;
		bool IsBlockEntity() const;
		std::unique_ptr<BlockEntity> CreateBlockEntityInstance(glm::ivec3 worldPosition);
		bool IsCutout() const;
		bool Translucent() const;
		std::vector<ImageID> GetTextures() const;
		void Render(BlockRenderContext& renderContext) const;


	private:
		uint8_t id;
		std::function<void(BlockRenderContext&)> render;
		std::function<std::vector<ImageID>()> get_textures;
		std::string type;
		std::optional<std::function<std::unique_ptr<BlockEntity>(glm::ivec3)>> create_block_entity;
		bool is_cutout;
		bool translucent;
	};

	class BlockBuilder
	{
	public:
		explicit BlockBuilder(const std::string& type, uint8_t& id);
		BlockBuilder& SetType(const std::string& type);
		BlockBuilder& SetBlockEntity(const std::function<std::unique_ptr<BlockEntity>(glm::ivec3)>& createBlockEntity);
		BlockBuilder& SetIsCutout();
		BlockBuilder& SetTranslucent();
		BlockBuilder& SetRender(std::function<void(BlockRenderContext&)>& render);
		Block Build() const;

	private:
		void RenderBlockDefaultCube(BlockRenderContext& renderContext) const;

		uint8_t id;
		std::string type;
		std::optional<std::function<std::unique_ptr<BlockEntity>(glm::ivec3)>> create_block_entity;
		bool is_cutout;
		bool translucent;
		std::optional<std::function<void(BlockRenderContext&)>> render;
	};

	class BlockRegistry
	{
	public:
		static BlockRegistry& GetInstance();

		static const Block& GetBlockByType(const std::string& type);
		static const Block& GetBlockByID(uint8_t id);
		static const std::array<std::shared_ptr<Block>, 256>& GetBlocks();
		static size_t LoadedBlockCount();

	private:
		BlockRegistry();

		void RegisterBlock(Block block);

		std::unordered_map<std::string, std::shared_ptr<Block>> block_map;
		std::array<std::shared_ptr<Block>, 256> blocks;
	};

}  // namespace Eon