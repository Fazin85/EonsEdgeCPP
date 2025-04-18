#pragma once

#include "cube_renderer.h"
#include "texture.h"
#include "player.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

namespace Eon {
	class FishRenderer {
	public:
		FishRenderer();

		void Render(Camera& camera, glm::vec3 playerPos, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, glm::vec3 color, float time);

	private:
		std::unique_ptr<Texture> fish_front_texture;
		std::unique_ptr<Texture> fish_bottom_texture;
		std::unique_ptr<Texture> fish_front_side_texture;
		std::unique_ptr<Texture> fish_top_texture;
		CubeRenderer cubeRenderer;
		DisplayList top_fin_list;
		DisplayList generic_list;
		DisplayList head_list;
	};
}