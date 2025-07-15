#include "quadtree_manager.h"

namespace Eon
{
	QuadtreeManager::QuadtreeManager()
	{
		AABB rootBounds = AABB(glm::dvec3(0.0), glm::dvec3(world_size, 1.0, world_size));
		root = std::make_shared<QuadtreeNode>(rootBounds, 0);

		UpdateQuadtree();
	}

	void QuadtreeManager::UpdateQuadtree()
	{
		if (root == nullptr) return;

		// Update the quadtree based on player position
		UpdateNode(root, glm::dvec2(player_position.x, player_position.z));

		// Update visualization
		/*if (showQuads)
		{
			UpdateVisualization(root);
		}*/
	}

	void QuadtreeManager::UpdateNode(std::shared_ptr<QuadtreeNode> node, glm::dvec2 playerPos2D)
	{
		if (node == nullptr) return;

		// Calculate distance from player to node center (only X and Z)
		glm::dvec2 nodeCenter2D = glm::dvec2(node->GetBounds().position.x, node->GetBounds().position.z);
		double distance = glm::distance(playerPos2D, nodeCenter2D);

		// Calculate subdivision and merge distances based on node size
		double nodeSize = std::max(node->GetBounds().dimensions.x, node->GetBounds().dimensions.z);

		double subdivisionDistance, mergeDistance;

		subdivisionDistance = nodeSize * subdivision_multiplier;
		mergeDistance = nodeSize * merge_multiplier;

		// Check if we should subdivide
		if (node->IsLeaf() && node->GetLevel() < max_level && distance < subdivisionDistance)
		{
			node->Subdivide();
		}
		// Check if we should merge
		else if (!node->IsLeaf() && distance > mergeDistance)
		{
			node->Merge();
		}

		// Recursively update children
		if (!node->IsLeaf())
		{
			for (int i = 0; i < 4; i++)
			{
				auto child = node->GetChild(i);
				if (child != nullptr)
				{
					UpdateNode(child, playerPos2D);
				}
			}
		}

		// Generate heightmap for leaf nodes
		/*if (node->IsLeaf() && generateHeightmap && node.heightmap == null)
		{
			GenerateHeightmap(node);
		}*/
	}

}