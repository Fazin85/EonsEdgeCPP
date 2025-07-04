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
	class Chunk;

	struct BlockRenderContext
	{
		Chunk& chunk;
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

	constexpr std::array<unsigned char, 12> GetFaceDataFromDirection(Directions dir)
	{
		switch (dir)
		{
		case Directions::Front:
			return std::array<unsigned char, 12>({ 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1 });
		case Directions::Back:
			return std::array<unsigned char, 12>({ 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0 });
		case Directions::Left:
			return std::array<unsigned char, 12>({ 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0 });
		case Directions::Right:
			return std::array<unsigned char, 12>({ 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1 });
		case Directions::Top:
			return std::array<unsigned char, 12>({ 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0 });
		case Directions::Bottom:
			return std::array<unsigned char, 12>({ 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1 });
		}

		return {};
	}

	constexpr std::array<glm::ivec3, 4> GetAOOffsets(Directions dir)
	{
		switch (dir)
		{
		case Eon::Directions::Front:
			return std::array<glm::ivec3, 4> {
				glm::ivec3(-1, -1, 1),
					glm::ivec3(1, -1, 1),
					glm::ivec3(1, 1, 1),
					glm::ivec3(-1, 1, 1)
			};
		case Eon::Directions::Back:
			return std::array<glm::ivec3, 4> {
				glm::ivec3(1, -1, -1),
					glm::ivec3(-1, -1, -1),
					glm::ivec3(-1, 1, -1),
					glm::ivec3(1, 1, -1)
			};
		case Eon::Directions::Left:
			return std::array<glm::ivec3, 4> {
				glm::ivec3(-1, -1, -1),
					glm::ivec3(-1, -1, 1),
					glm::ivec3(-1, 1, 1),
					glm::ivec3(-1, 1, -1)
			};
		case Eon::Directions::Right:
			return std::array<glm::ivec3, 4> {
				glm::ivec3(1, -1, 1),
					glm::ivec3(1, -1, -1),
					glm::ivec3(1, 1, -1),
					glm::ivec3(1, 1, 1)
			};
		case Eon::Directions::Top:
			return std::array<glm::ivec3, 4> {
				glm::ivec3(-1, 1, 1),
					glm::ivec3(1, 1, 1),
					glm::ivec3(1, 1, -1),
					glm::ivec3(-1, 1, -1)
			};
		case Eon::Directions::Bottom:
			return std::array<glm::ivec3, 4> {
				glm::ivec3(-1, -1, -1),
					glm::ivec3(1, -1, -1),
					glm::ivec3(1, -1, 1),
					glm::ivec3(-1, -1, 1)
			};
		default:
			return std::array<glm::ivec3, 4> {};
		}
	}

	void AddFace(BlockRenderContext& renderContext, Directions direction);
	int GetAmbientOcclusion(const Block& corner, const Block& side1, const Block& side2);
	void CalculateAO(Chunk& chunk, int x, int y, int z, std::array<glm::ivec3, 4>& offsets, int side, std::array<int, 4>& aos);

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
				BlockRenderContext&)>& render,
			bool solid,
			float shininess);

		bool operator==(const Block& other) const;

		uint8_t GetID() const;
		const std::string& GetType() const;
		bool IsBlockEntity() const;
		std::unique_ptr<BlockEntity> CreateBlockEntityInstance(glm::ivec3 worldPosition);
		bool IsCutout() const;
		bool Translucent() const;
		bool Solid() const;
		void Render(BlockRenderContext& renderContext) const;
		ImageID GetTexture(Directions dir) const;
		float GetShininess() const;

	private:
		uint8_t id;
		std::function<void(BlockRenderContext&)> render;
		std::string type;
		std::optional<std::function<std::unique_ptr<BlockEntity>(glm::ivec3)>> create_block_entity;
		bool is_cutout;
		bool translucent;
		bool solid;
		float shininess;
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
		BlockBuilder& SetSolid(bool solid);
		BlockBuilder& SetShininess(float shininess);
		Block Build() const;

	private:
		void RenderBlockDefaultCube(BlockRenderContext& renderContext) const;

		uint8_t id;
		std::string type;
		std::optional<std::function<std::unique_ptr<BlockEntity>(glm::ivec3)>> create_block_entity;
		bool is_cutout;
		bool translucent;
		bool solid;
		float shininess;
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