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

		ChunkPosition(int x, int z, int lod) : x((x >> CHUNK_BITSHIFT_AMOUNT)* CHUNK_WIDTH), z((z >> CHUNK_BITSHIFT_AMOUNT)* CHUNK_WIDTH), lod(lod)
		{
		}

		int x;
		int z;
		int lod = 1;

		bool operator==(ChunkPosition const& rhs) const = default;

		bool operator<(const ChunkPosition& other) const
		{
			if (lod != other.lod) return lod < other.lod;
			if (x != other.x) return x < other.x;
			return z < other.z;
		}

		ChunkPosition Offset(int x, int z) const
		{
			return ChunkPosition{ this->x + x, this->z + z };
		}

		ChunkPosition GetParent() const
		{
			int parentLod = lod * 2;
			int parentSize = CHUNK_WIDTH * parentLod;

			// Snap to parent grid
			int parentX = (x / parentSize) * parentSize;
			int parentZ = (z / parentSize) * parentSize;

			return ChunkPosition(parentX, parentZ, parentLod);
		}

		std::vector<ChunkPosition> GetChildren() const
		{
			if (lod == 1) return {}; // Base level has no children

			int childLod = lod / 2;
			int childSize = CHUNK_WIDTH * childLod;

			return {
				ChunkPosition(x, z, childLod),
				ChunkPosition(x + childSize, z, childLod),
				ChunkPosition(x, z + childSize, childLod),
				ChunkPosition(x + childSize, z + childSize, childLod)
			};
		}

		int GetWorldSize() const { return CHUNK_WIDTH * lod; }

		glm::ivec2 GetWorldMin() const { return glm::ivec2(x, z); }
		glm::ivec2 GetWorldMax() const { return glm::ivec2(x + GetWorldSize(), z + GetWorldSize()); }

		bool Contains(glm::ivec2 worldPos) const
		{
			glm::ivec2 min = GetWorldMin();
			glm::ivec2 max = GetWorldMax();
			return worldPos.x >= min.x && worldPos.x < max.x &&
				worldPos.y >= min.y && worldPos.y < max.y;
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