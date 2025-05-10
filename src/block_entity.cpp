#include "block_entity.h"

namespace Eon
{
	BlockEntity::BlockEntity(const glm::ivec3& worldPosition) : world_position(worldPosition)
	{
	}

	glm::ivec3 BlockEntity::GetWorldPosition() const
	{
		return world_position;
	}
}