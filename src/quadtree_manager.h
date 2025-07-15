#pragma once

#include "quadtree_node.h"

namespace Eon
{
	class QuadtreeManager
	{
	public:
		QuadtreeManager();

		void UpdateQuadtree();

		void UpdateNode(std::shared_ptr<QuadtreeNode> node, glm::dvec2 playerPos2D);

	private:
		glm::dvec3 player_position = { 0, 0, 0 };
		float world_size = 1000.0f;
		int max_level = 6;

		bool use_automatic_distances = true;
		float subdivision_multiplier = 1.5f; // Subdivide when player is within 1.5x the node size
		float merge_multiplier = 2.5f; // Merge when player is beyond 2.5x the node size

		std::shared_ptr<QuadtreeNode> root = nullptr;
		//Transform quadContainer;
	};
}