#pragma once

#include "block_entity.h"

namespace Eon
{
	class BlockEntityTest : public BlockEntity
	{
	public:
		using BlockEntity::BlockEntity;

		void Render(const Camera& camera) override;
	};
}