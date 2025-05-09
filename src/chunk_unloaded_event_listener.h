#pragma once

#include "chunk.h"

namespace Eon
{
	class ChunkUnloadedEventListener
	{
	public:
		virtual ~ChunkUnloadedEventListener() = default;
		virtual void OnChunkUnloaded(std::shared_ptr<Chunk> chunk) = 0;
	};
}