#pragma once

#include "chunk_info.h"

#include <functional>

namespace Eon
{
	struct ChunkPosition
	{
		ChunkPosition() : x(0), z(0)
		{
		}

		ChunkPosition(int x, int z) : x((x >> CHUNK_BITSHIFT_AMOUNT)* CHUNK_WIDTH), z((z >> CHUNK_BITSHIFT_AMOUNT)* CHUNK_WIDTH)
		{
		}

		int x;
		int z;

		bool operator==(ChunkPosition const& rhs) const = default;

		ChunkPosition Offset(int x, int z) const
		{
			return ChunkPosition{ this->x + x, this->z + z };
		}
	};
}

namespace std
{
	template <> struct hash<Eon::ChunkPosition>
	{
		size_t operator()(const Eon::ChunkPosition& vect) const noexcept
		{
			std::hash<decltype(vect.x)> hasher;

			auto hash1 = hasher(vect.x);
			auto hash2 = hasher(vect.z);

			return std::hash<decltype(vect.x)>{}((hash1 ^ hash2) >> 2);
		}
	};
} // namespace std