#pragma once

#include "block_texture_provider.h"
#include "texture_atlas_stitcher.h"

namespace Eon
{
	class DefaultBlockTextureProvider : public BlockTextureProvider
	{
	public:
		DefaultBlockTextureProvider();

		glm::vec4 GetUVs(const Block& block, Directions dir) const override;
		TextureID GetBlockAtlas() const override;
		TextureID GetBlockIDAtlas() const override;

	private:
		std::shared_ptr<sf::Image> CreateBlockImage(int width, int height, const Block& block) const;
		
		std::vector<std::vector<glm::vec4>> uvs;
		TextureID block_atlas;
		TextureID block_id_atlas;
	};
}