#pragma once

#include <array>
#include <memory>
#include <vector>

#include "chunk_renderer.h"

namespace Eon {
	class LevelRenderer {
	public:
		LevelRenderer();

	private:
		std::vector<std::shared_ptr<ChunkRenderer>> chunk_renderers;
	};
}  // namespace Eon