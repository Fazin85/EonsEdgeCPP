#pragma once

#include "block_texture_provider.h"
#include "texture_atlas_stitcher.h"

namespace Eon
{
	class DefaultBlockTextureProvider : public BlockTextureProvider
	{
	public:
		explicit DefaultBlockTextureProvider(TextureAtlasStitcher& stitcher);

		glm::vec4 GetUVs(const Block& block, int index) const override;
		TextureID GetBlockAtlas() const override;

	private:
		std::vector<std::vector<glm::vec4>> uvs;
		TextureID block_atlas;
	};
}