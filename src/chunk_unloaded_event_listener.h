#pragma once

#include "chunk.h"

namespace Eon {
	class ChunkUnloadedEventListener {
	public:
		virtual ~ChunkUnloadedEventListener() = default;
		virtual void OnChunkUnloaded(Chunk& chunk) = 0;
	};
}