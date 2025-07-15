#include "quadtree_node.h"

namespace Eon
{
	QuadtreeNode::QuadtreeNode(AABB bounds, int level) : bounds(bounds), level(level)
	{
		node_count++;
	}

	QuadtreeNode::~QuadtreeNode()
	{
		node_count--;
	}

	AABB& QuadtreeNode::GetBounds()
	{
		return bounds;
	}

	int QuadtreeNode::GetLevel() const
	{
		return level;
	}

	bool QuadtreeNode::IsLeaf() const
	{
		return is_leaf;
	}

	std::shared_ptr<Mesh> QuadtreeNode::GetMesh()
	{
		return mesh;
	}

	void QuadtreeNode::SetMesh(std::shared_ptr<Mesh> mesh)
	{
		this->mesh = mesh;
	}

	std::shared_ptr<QuadtreeNode> QuadtreeNode::GetChild(int index)
	{
		return children[index];
	}

	void QuadtreeNode::Subdivide()
	{
		if (!is_leaf) return;

		is_leaf = false;

		glm::dvec3 center = bounds.position;
		glm::dvec3 size = bounds.dimensions;
		double halfWidth = size.x * 0.5;
		double halfHeight = size.z * 0.5;

		// Create 4 child nodes
		// Bottom-left
		children[0] = std::make_shared<QuadtreeNode>(AABB(
			glm::dvec3(center.x - halfWidth * 0.5, center.y, center.z - halfHeight * 0.5),
			glm::dvec3(halfWidth, size.y, halfHeight)
		), level + 1);

		// Bottom-right
		children[1] = std::make_shared<QuadtreeNode>(AABB(
			glm::dvec3(center.x + halfWidth * 0.5, center.y, center.z - halfHeight * 0.5),
			glm::dvec3(halfWidth, size.y, halfHeight)
		), level + 1);

		// Top-left
		children[2] = std::make_shared<QuadtreeNode>(AABB(
			glm::dvec3(center.x - halfWidth * 0.5, center.y, center.z + halfHeight * 0.5),
			glm::dvec3(halfWidth, size.y, halfHeight)
		), level + 1);

		// Top-right
		children[3] = std::make_shared<QuadtreeNode>(AABB(
			glm::dvec3(center.x + halfWidth * 0.5, center.y, center.z + halfHeight * 0.5),
			glm::dvec3(halfWidth, size.y, halfHeight)
		), level + 1);
	}

	void QuadtreeNode::Merge()
	{
		if (is_leaf) return;

		is_leaf = true;

		for (int i = 0; i < 4; i++)
		{
			if (children[i] != nullptr)
			{
				children[i]->Destroy();
				children[i].reset();
				children[i] = nullptr;
			}
		}
	}

	void QuadtreeNode::Destroy()
	{
		if (mesh != nullptr)
		{
			mesh.reset();
			mesh = nullptr;
		}

		if (!is_leaf)
		{
			for (int i = 0; i < 4; i++)
			{
				if (children[i] != nullptr)
				{
					children[i]->Destroy();
				}
			}
		}
	}

	int QuadtreeNode::GetNodeCount()
	{
		return node_count;
	}
}

