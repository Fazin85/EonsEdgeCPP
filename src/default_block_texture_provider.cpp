#include "default_block_texture_provider.h"
#include "block.h"
#include "log.h"
#include "assert.h"
#include <format>

namespace Eon
{
	DefaultBlockTextureProvider::DefaultBlockTextureProvider()
	{
		TextureAtlasStitcher albedoStitcher;
		TextureAtlasStitcher blockIDStitcher;

		const std::array<std::shared_ptr<Block>, 256>& blockArray = BlockRegistry::GetBlocks();
		auto blocks = std::vector(blockArray.begin(), blockArray.begin() + BlockRegistry::LoadedBlockCount());

		uvs.resize(blocks.size());

		for (int i = 0; i < blocks.size(); i++)
		{
			uvs[i].resize(6);
		}

		std::vector<std::shared_ptr<sf::Image>> blockIDImagePtrs; // we must keep these alive
		std::vector<ImageID> ids;
		std::unordered_map<ImageID, std::string> idToNameMap;
		for (const auto& block : blocks)
		{
			for (int i = 0; i < 6; i++)
			{
				auto direction = static_cast<Directions>(i);
				if (std::find(ids.begin(), ids.end(), block->GetTexture(direction)) == ids.end())
				{
					ImageID texture = block->GetTexture(direction);
					std::string name = std::format("block_texture{}", ids.size());
					auto image = texture.Get<sf::Image>();
					albedoStitcher.AddSprite(name, *image);
					ids.emplace_back(texture);
					idToNameMap[texture] = name;

					auto blockIDImage = CreateBlockImage(image->getSize().x, image->getSize().y, *block);

					blockIDStitcher.AddSprite(name, *blockIDImage);
					blockIDImagePtrs.emplace_back(blockIDImage);
				}
			}
		}

		albedoStitcher.DoStitch();
		blockIDStitcher.DoStitch();

		EON_ASSERT(albedoStitcher.GetCurrentSize() == blockIDStitcher.GetCurrentSize(), "Atlas sizes do not match");

		glm::ivec2 stitchSize = albedoStitcher.GetCurrentSize();
		for (const auto& block : blocks)
		{
			for (int i = 0; i < 6; i++)
			{
				auto direction = static_cast<Directions>(i);
				auto holder = albedoStitcher.GetHolder(idToNameMap[block->GetTexture(direction)]);
				uvs[block->GetID()][i] = glm::vec4
				(
					holder->GetU0(static_cast<float>(stitchSize.x)),
					holder->GetV0(static_cast<float>(stitchSize.y)),
					holder->GetU1(static_cast<float>(stitchSize.x)),
					holder->GetV1(static_cast<float>(stitchSize.y))
				);
			}
		}

#ifdef _DEBUG
		auto img = albedoStitcher.StitchImages();
		img->saveToFile("block_atlas.png");
		auto idImg = blockIDStitcher.StitchImages();
		idImg->saveToFile("block_id_atlas.png");
#endif

		auto blockAtlas = make_shared_st<Texture>(std::move(*albedoStitcher.CreateAtlasTexture().release()));
		block_atlas = AssetManager::CreateAsset("texture.block_atlas", blockAtlas);

		auto blockIDAtlas = make_shared_st<Texture>(std::move(*blockIDStitcher.CreateAtlasTexture().release()));
		block_id_atlas = AssetManager::CreateAsset("texture.block_id_atlas", blockIDAtlas);
	}

	glm::vec4 DefaultBlockTextureProvider::GetUVs(const Block& block, Directions dir) const
	{
		return uvs[block.GetID()][static_cast<int>(dir)];
	}

	TextureID DefaultBlockTextureProvider::GetBlockAtlas() const
	{
		return block_atlas;
	}

	TextureID DefaultBlockTextureProvider::GetBlockIDAtlas() const
	{
		return block_id_atlas;
	}

	std::shared_ptr<sf::Image> DefaultBlockTextureProvider::CreateBlockImage(int width, int height, const Block& block) const
	{
		auto blockIDImage = std::make_shared<sf::Image>();
		blockIDImage->create(width, height, sf::Color::Black);
		for (unsigned int x = 0; x < blockIDImage->getSize().x; x++)
		{
			for (unsigned int y = 0; y < blockIDImage->getSize().y; y++)
			{
				sf::Color materialPixel(block.GetID(), static_cast<sf::Uint8>(block.GetShininess()), 0, 255);
				blockIDImage->setPixel(x, y, materialPixel);
			}
		}

		return blockIDImage;
	}
}