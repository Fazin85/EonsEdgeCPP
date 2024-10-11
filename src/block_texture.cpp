#include "block_texture.h"

namespace Eon
{
	std::map<const Directions, const glm::vec2> Eon::BlockTexture::GetUVCoordsFromBlockID(BlockType type)
	{
		switch (type)
		{
		case BlockType::STONE:
			return
			{
				{Directions::Front, {1, 15}},
				{Directions::Back, {1, 15}},
				{Directions::Left, {1, 15}},
				{Directions::Right, {1, 15}},
				{Directions::Top, {1, 15}},
				{Directions::Bottom, {1, 15}}
			};
		case BlockType::DIRT:
			return
			{
				{Directions::Front, {2, 15}},
				{Directions::Back, {2, 15}},
				{Directions::Left, {2, 15}},
				{Directions::Right, {2, 15}},
				{Directions::Top, {2, 15}},
				{Directions::Bottom, {2, 15}}
			};
		case BlockType::GRASS:
			return
			{
				{Directions::Front, {3, 15}},
				{Directions::Back, {3, 15}},
				{Directions::Left, {3, 15}},
				{Directions::Right, {3, 15}},
				{Directions::Top, {3, 14}},
				{Directions::Bottom, {2, 15}}
			};
		}

		return {};
	}
}
