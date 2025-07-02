#include "default_block_texture_provider.h"
#include "block.h"
#include "log.h"
#include <format>

namespace Eon
{
	DefaultBlockTextureProvider::DefaultBlockTextureProvider(TextureAtlasStitcher& stitcher)
	{
		const std::array<std::shared_ptr<Block>, 256>& blockArray = BlockRegistry::GetBlocks();
		auto blocks = std::vector(blockArray.begin(), blockArray.begin() + BlockRegistry::LoadedBlockCount());

		uvs.resize(blocks.size());

		for (int i = 0; i < blocks.size(); i++)
		{
			uvs[i].resize(blocks[i]->GetTextures().size());
		}

		int textureId = 0;
		for (const auto& block : blocks)
		{
			auto textures = block->GetTextures();
			for (const auto& texture : textures)
			{
				std::string textureName = std::format("block_texture{}", textureId++);
				stitcher.AddSprite(textureName, *texture.Get<sf::Image>());
			}
		}

		stitcher.DoStitch();

		glm::ivec2 stitchSize = stitcher.GetCurrentSize();
		textureId = 0;
		for (int i = 0; i < blocks.size(); i++)
		{
			auto textures = blocks[i]->GetTextures();
			for (int j = 0; j < textures.size(); j++)
			{
				std::string textureName = std::format("block_texture{}", textureId++);
				auto holder = stitcher.GetHolder(textureName);
				uvs[i][j] = glm::vec4
				(
					holder->GetU0(static_cast<float>(stitchSize.x)),
					holder->GetV0(static_cast<float>(stitchSize.y)),
					holder->GetU1(static_cast<float>(stitchSize.x)),
					holder->GetV1(static_cast<float>(stitchSize.y))
				);
			}
		}
		auto img = stitcher.StitchImages();
		img->saveToFile("block_atlas.png");
		auto blockAtlas = make_shared_st<Texture>(std::move(*stitcher.CreateAtlasTexture().release()));

		auto asset = AssetManager::CreateAsset("texture.block_atlas", blockAtlas);

		block_atlas = asset;
	}

	glm::vec4 DefaultBlockTextureProvider::GetUVs(const Block& block, int index) const
	{
		return uvs[block.GetID()][index];
	}

	TextureID DefaultBlockTextureProvider::GetBlockAtlas() const
	{
		return block_atlas;
	}
}