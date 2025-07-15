#pragma once

#include <array>
#include <memory>
#include <atomic>
#include "aabb.h"
#include "mesh.h"

namespace Eon
{
	class QuadtreeNode
	{
	public:
		QuadtreeNode(AABB bounds, int level);

		~QuadtreeNode();

		AABB& GetBounds();
		int GetLevel() const;
		bool IsLeaf() const;

		std::shared_ptr<Mesh> GetMesh();
		void SetMesh(std::shared_ptr<Mesh> mesh);

		std::shared_ptr<QuadtreeNode> GetChild(int index);

		void Subdivide();
		void Merge();

		void Destroy();

		static int GetNodeCount();

	private:
		static inline std::atomic<int> node_count = 0;

		std::shared_ptr<Mesh> mesh = nullptr;
		AABB bounds;
		int level;
		bool is_leaf = true;
		std::array<std::shared_ptr<QuadtreeNode>, 4> children = { nullptr, nullptr, nullptr, nullptr };
	};
}