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
			uvs[i].resize(blocks[i]->GetTextures().size());
		}

		std::vector<std::shared_ptr<sf::Image>> blockIDImagePtrs; // we must keep these alive

		int textureId = 0;
		for (const auto& block : blocks)
		{
			auto textures = block->GetTextures();
			for (const auto& texture : textures)
			{
				std::string textureName = std::format("block_texture{}", textureId++);
				auto image = texture.Get<sf::Image>();

				albedoStitcher.AddSprite(textureName, *image);

				auto blockIDImage = std::make_shared<sf::Image>();
				blockIDImage->create(image->getSize().x, image->getSize().y, sf::Color::Black);
				for (unsigned int x = 0; x < blockIDImage->getSize().x; x++)
				{
					for (unsigned int y = 0; y < blockIDImage->getSize().y; y++)
					{
						sf::Color materialPixel(block->GetID(), 0, 0, 255);
						blockIDImage->setPixel(x, y, materialPixel);
					}
				}

				blockIDStitcher.AddSprite(textureName, *blockIDImage);
				blockIDImagePtrs.emplace_back(blockIDImage);
			}
		}

		albedoStitcher.DoStitch();
		blockIDStitcher.DoStitch();

		EON_ASSERT(albedoStitcher.GetCurrentSize() == blockIDStitcher.GetCurrentSize(), "Atlas sizes do not match");

		glm::ivec2 stitchSize = albedoStitcher.GetCurrentSize();
		textureId = 0;
		for (int i = 0; i < blocks.size(); i++)
		{
			auto textures = blocks[i]->GetTextures();
			for (int j = 0; j < textures.size(); j++)
			{
				std::string textureName = std::format("block_texture{}", textureId++);
				auto holder = albedoStitcher.GetHolder(textureName);
				uvs[i][j] = glm::vec4
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

	glm::vec4 DefaultBlockTextureProvider::GetUVs(const Block& block, int index) const
	{
		return uvs[block.GetID()][index];
	}

	TextureID DefaultBlockTextureProvider::GetBlockAtlas() const
	{
		return block_atlas;
	}

	TextureID DefaultBlockTextureProvider::GetBlockIDAtlas() const
	{
		return block_id_atlas;
	}
}