#pragma once

#include "block_texture_provider.h"
#include "texture_atlas_stitcher.h"

namespace Eon
{
	class DefaultBlockTextureProvider : public BlockTextureProvider
	{
	public:
		DefaultBlockTextureProvider();

		glm::vec4 GetUVs(const Block& block, int index) const override;
		TextureID GetBlockAtlas() const override;
		TextureID GetBlockIDAtlas() const override;

	private:
		std::vector<std::vector<glm::vec4>> uvs;
		TextureID block_atlas;
		TextureID block_id_atlas;
	};
}