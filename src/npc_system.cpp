#include "npc_system.h"

namespace Eon {
	NPCSystem::NPCSystem() : npcs()
	{
		next_spawn_index = 0;
	}

	int NPCSystem::NewNPC(NPCType type, glm::vec3 position, glm::vec3 rotation)
	{
		for (int i = 0; i < npcs.size(); i++) {
			if (!npcs[i].active) {
				npcs[i].index = i;
				npcs[i].OnSpawn(type, position, rotation);
				npcs[i].active = true;

				return i;
			}
		}

		int i = next_spawn_index;

		npcs[i].index = i;
		npcs[i].OnSpawn(type, position, rotation);
		npcs[i].active = true;

		next_spawn_index++;
		if (next_spawn_index >= npcs.size()) {
			next_spawn_index = 0;
		}

		return i;
	}

	void NPCSystem::Tick()
	{
		for (NPC& npc : npcs) {
			npc.Tick();
		}
	}

	void NPCSystem::Render(Camera& camera, glm::vec3 cameraPosition)
	{
		camera.CalculateViewMatrix(cameraPosition);

		for (NPC& npc : npcs) {
			if (npc.active && camera.GetFrustum().BoxInFrustum(npc.GetRenderAABB())) {
				npc.Render(camera, cameraPosition);
			}
		}
	}

	NPC& NPCSystem::AtIndex(int index)
	{
		if (index >= npcs.size() || index < 0) {
			throw std::invalid_argument("NPCSystem::AtIndex::index out of bounds");
		}

		return npcs[index];
	}
}
