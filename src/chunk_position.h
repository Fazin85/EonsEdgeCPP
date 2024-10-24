#pragma once


#include <functional>

namespace Eon
{
	struct ChunkPosition
	{
		unsigned char x;
		unsigned char z;

		bool operator==(ChunkPosition const& rhs) const
		{
			return x == rhs.x && z == rhs.z;
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