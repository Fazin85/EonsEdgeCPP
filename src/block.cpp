#include "block.h"
#include "block_texture_provider.h"
#include "assert.h"
#include "chunk.h"

namespace Eon
{
	Block::Block(
		uint8_t id,
		const std::string& type,
		const std::optional<std::function<std::unique_ptr<BlockEntity>(glm::ivec3)>>& createBlockEntity,
		bool isCutout,
		bool translucent,
		std::function<void(BlockRenderContext&)>& render,
		bool solid) :
		id(id),
		type(type),
		create_block_entity(std::nullopt),
		is_cutout(isCutout),
		translucent(translucent),
		render(render),
		solid(solid)
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

	const std::string& Block::GetType() const
	{
		return type;
	}

	bool Block::IsBlockEntity() const
	{
		return create_block_entity.has_value();
	}

	std::unique_ptr<BlockEntity> Block::CreateBlockEntityInstance(glm::ivec3 worldPosition)
	{
		const auto& createBlockEntity = create_block_entity.value();
		return createBlockEntity(worldPosition);
	}

	bool Block::IsCutout() const
	{
		return is_cutout;
	}

	bool Block::Translucent() const
	{
		return translucent;
	}

	bool Block::Solid() const
	{
		return solid;
	}

	std::vector<ImageID> Block::GetTextures() const
	{
		return { AssetManager::GetAssetID<sf::Image>("block.stone") };
	}

	void Block::Render(BlockRenderContext& renderContext) const
	{
		render(renderContext);
	}

	BlockBuilder::BlockBuilder(const std::string& type, uint8_t& i) : id(i++), type(type), is_cutout(false), translucent(false), solid(true)
	{
	}

	BlockBuilder& BlockBuilder::SetType(const std::string& t)
	{
		type = t;
		return *this;
	}

	BlockBuilder& BlockBuilder::SetBlockEntity(const std::function<std::unique_ptr<BlockEntity>(glm::ivec3)>& createBlockEntity)
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

	BlockBuilder& BlockBuilder::SetRender(std::function<void(BlockRenderContext&)>& render)
	{
		this->render = render;
		return *this;
	}

	BlockBuilder& BlockBuilder::SetSolid(bool solid)
	{
		this->solid = solid;
		return *this;
	}

	Block BlockBuilder::Build() const
	{
		auto renderFunc = render.value_or(
			std::function<void(BlockRenderContext&)>(
				[this](BlockRenderContext& renderContext) { RenderBlockDefaultCube(renderContext); }
			)
		);

		return Block(id, type, create_block_entity, is_cutout, translucent, renderFunc, solid);
	}

	void BlockBuilder::RenderBlockDefaultCube(BlockRenderContext& renderContext) const
	{
		auto transparent = [](const Block& sideBlock, const Block& block)
			{
				return sideBlock.IsCutout() || (!block.Translucent() && sideBlock.Translucent());
			};

		if (transparent(renderContext.topBlock, renderContext.centerBlock))
		{
			AddFace(renderContext, Directions::Top);
		}

		if (transparent(renderContext.bottomBlock, renderContext.centerBlock))
		{
			AddFace(renderContext, Directions::Bottom);
		}

		if (transparent(renderContext.leftBlock, renderContext.centerBlock))
		{
			AddFace(renderContext, Directions::Left);
		}

		if (transparent(renderContext.rightBlock, renderContext.centerBlock))
		{
			AddFace(renderContext, Directions::Right);
		}

		if (transparent(renderContext.frontBlock, renderContext.centerBlock))
		{
			AddFace(renderContext, Directions::Front);
		}

		if (transparent(renderContext.backBlock, renderContext.centerBlock))
		{
			AddFace(renderContext, Directions::Back);
		}
	}

	BlockRegistry::BlockRegistry() : blocks()
	{
		uint8_t id = 0;
		RegisterBlock(BlockBuilder("Air", id).SetIsCutout().SetSolid(false).Build());
		RegisterBlock(BlockBuilder("Stone", id).Build());
		RegisterBlock(BlockBuilder("Grass", id).Build());
		RegisterBlock(BlockBuilder("Dirt", id).Build());
		RegisterBlock(BlockBuilder("Water", id).SetTranslucent().SetSolid(false).Build());
		RegisterBlock(BlockBuilder("Sand", id).Build());
		RegisterBlock(BlockBuilder("OakLog", id).Build());
		RegisterBlock(BlockBuilder("Leaf", id).SetIsCutout().Build());
		RegisterBlock(BlockBuilder("Gravel", id).Build());
	}

	BlockRegistry& BlockRegistry::GetInstance()
	{
		static BlockRegistry instance;
		return instance;
	}

	const Block& BlockRegistry::GetBlockByType(const std::string& type)
	{
		return *GetInstance().block_map.at(type);
	}

	const Block& BlockRegistry::GetBlockByID(uint8_t id)
	{
		return *GetInstance().blocks[id].get();
	}

	const std::array<std::shared_ptr<Block>, 256>& BlockRegistry::GetBlocks()
	{
		return GetInstance().blocks;
	}

	size_t BlockRegistry::LoadedBlockCount()
	{
		return GetInstance().block_map.size();
	}

	void BlockRegistry::RegisterBlock(Block block)
	{
		auto ptr = std::make_shared<Block>(block);
		block_map[block.GetType()] = ptr;
		blocks[block.GetID()] = ptr;
	}

	std::array<unsigned char, 12> GetFaceDataFromDirection(Directions dir)
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

	std::array<glm::ivec3, 4> GetAOOffsets(Directions dir)
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

	void AddFace(BlockRenderContext& renderContext, Directions direction)
	{
		auto uvValue = [](glm::vec4 uvs, int index) constexpr
			{
				EON_ASSERT(index >= 0 && index < 4, "Invalid index");

				switch (index)
				{
				case 0:
					return glm::vec2(uvs.x, uvs.w);
				case 1:
					return glm::vec2(uvs.z, uvs.w);
				case 2:
					return glm::vec2(uvs.z, uvs.y);
				case 3:
					return glm::vec2(uvs.x, uvs.y);
				}
				return glm::vec2(0.0f, 0.0f);
			};

		auto faceData = GetFaceDataFromDirection(direction);
		//glm::vec4 uvs = renderContext.uvProvider.GetUVs(renderContext.centerBlock, static_cast<int>(direction));
		glm::vec4 uvs = renderContext.uvProvider.GetUVs(renderContext.centerBlock, 0);

		auto side = [](Directions dir) constexpr
			{
				using enum Directions;
				switch (dir)
				{
				case Front:
				case Back:
					return 2;
				case Left:
				case Right:
					return 0;
				case Top:
				case Bottom:
					return 1;
				default:
					return 0;
				}
			};

		std::array<int, 4> aos{};
		std::array<glm::ivec3, 4> offsets = GetAOOffsets(direction);
		CalculateAO(
			renderContext.chunk,
			renderContext.blockPosition.x,
			renderContext.blockPosition.y,
			renderContext.blockPosition.z,
			offsets,
			side(direction),
			aos);

		constexpr static std::array<float, 4> aoMapping{ 1.0f, 0.9f, 0.7f, 0.5f };

		std::array<glm::vec3, 4> faceVertices{};
		std::array<glm::vec2, 4> faceUVs{};

		int index = 0;
		for (int i = 0; i < 4; i++)
		{
			int x = faceData[index++] + renderContext.blockPosition.x;
			int y = faceData[index++] + renderContext.blockPosition.y;
			int z = faceData[index++] + renderContext.blockPosition.z;

			faceVertices[i] = glm::vec3(x, y, z);
			faceUVs[i] = uvValue(uvs, i);
		}

		// Add first triangle (0, 1, 2)
		for (int i : {0, 1, 2})
		{
			renderContext.meshData.vertexPositions.push_back(faceVertices[i]);
			renderContext.meshData.uvs.push_back(faceUVs[i]);
			renderContext.meshData.colors.push_back(glm::vec3(1.0f) * aoMapping[aos[i]]);
			renderContext.meshData.normals.push_back(GetNormalFromDirection(direction));
		}

		// Add second triangle (0, 2, 3)  
		for (int i : {0, 2, 3})
		{
			renderContext.meshData.vertexPositions.push_back(faceVertices[i]);
			renderContext.meshData.uvs.push_back(faceUVs[i]);
			renderContext.meshData.colors.push_back(glm::vec3(1.0f) * aoMapping[aos[i]]);
			renderContext.meshData.normals.push_back(GetNormalFromDirection(direction));
		}
	}

	int GetAmbientOcclusion(const Block& corner, const Block& side1, const Block& side2)
	{
		if (side1.Solid() && side2.Solid())
		{
			return 3;
		}
		else if (corner.Solid() && (side1.Solid() || side2.Solid()))
		{
			return 2;
		}
		else if (corner.Solid() || side1.Solid() || side2.Solid())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	void CalculateAO(Chunk& chunk, int x, int y, int z, std::array<glm::ivec3, 4>& offsets, int side, std::array<int, 4>& aos)
	{
		std::array<const Block*, 4> corner{};
		std::array<const Block*, 4> side1{};
		std::array<const Block*, 4> side2{};

		for (int i = 0; i < 4; i++)
		{
			glm::ivec3 offset = offsets[i];

			corner[i] = &chunk.GetBlock(x + offset.x, y + offset.y, z + offset.z);

			switch (side)
			{
			case 0:
				side1[i] = &chunk.GetBlock(x + offset.x, y + offset.y, z);
				side2[i] = &chunk.GetBlock(x + offset.x, y, z + offset.z);
				break;
			case 1:
				side1[i] = &chunk.GetBlock(x + offset.x, y + offset.y, z);
				side2[i] = &chunk.GetBlock(x, y + offset.y, z + offset.z);
				break;
			case 2:
				side1[i] = &chunk.GetBlock(x + offset.x, y, z + offset.z);
				side2[i] = &chunk.GetBlock(x, y + offset.y, z + offset.z);
				break;
			default:
				break;
			}
		}

		for (int i = 0; i < 4; i++)
		{
			aos[i] = GetAmbientOcclusion(*corner[i], *side1[i], *side2[i]);
		}
	}
}