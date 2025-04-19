#pragma once

#include "npc.h"
#include "npc_type.h"

#include <glm/glm.hpp>
#include <array>

#define MAX_NPCS 200

namespace Eon {
	class NPCSystem {
	public:
		NPCSystem();

		int NewNPC(NPCType type, glm::vec3 position, glm::vec3 rotation);

		void Tick();
		void Render(Camera& camera, glm::vec3 cameraPosition);
		NPC& AtIndex(int index);

	private:
		std::array<NPC, MAX_NPCS> npcs;
		int next_spawn_index;
	};
}