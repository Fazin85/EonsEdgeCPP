#pragma once

#include "glm/glm.hpp"
#include "npc_type.h"
#include "fish_renderer.h"

namespace Eon {
	class NPC {
	public:
		NPC();

		void OnSpawn(NPCType type, glm::vec3 position, glm::vec3 rotation);
		int MaxLife() const;
		AABB& GetRenderAABB();

		void Tick();
		void Render(Camera& camera, glm::vec3 cameraPosition);

		bool active;
		int index;
		int life;
		glm::vec3 color;

	private:
		void InitRenderers();

		NPCType type;
		glm::vec3 position;
		glm::vec3 rotation;

		int max_life;
		long ticks_alive;
		AABB render_aabb;

		static std::unique_ptr<FishRenderer> fish_renderer;
	};
}