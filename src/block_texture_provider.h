#pragma once

#include "glm/glm.hpp"
#include "asset_manager.h"
#include "directions.h"

namespace Eon
{
	class Block;

	class BlockTextureProvider
	{
	public:
		virtual ~BlockTextureProvider() = default;
		virtual glm::vec4 GetUVs(const Block& block, Directions dir) const = 0;
		virtual TextureID GetBlockAtlas() const = 0;
		virtual TextureID GetBlockIDAtlas() const = 0;
	};
}
