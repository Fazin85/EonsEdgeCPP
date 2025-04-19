#include "npc.h"

namespace Eon {
	std::unique_ptr<FishRenderer> NPC::fish_renderer = nullptr;

	NPC::NPC() : type(NPCType::NONE), active(false), color(1.0f, 1.0f, 1.0f), index(-1), position(0, 0, 0), rotation(0, 0, 0), life(0), max_life(0), ticks_alive(0), render_aabb({ 0, 0, 0 })
	{
	}

	void NPC::OnSpawn(NPCType type, glm::vec3 position, glm::vec3 rotation)
	{
		InitRenderers();

		this->position = position;
		this->rotation = rotation;
		this->type = type;

		switch (type)
		{
		case NPCType::NONE:
			max_life = 0;
			render_aabb = { {0, 0, 0} };
			break;

		case NPCType::FISH:
			max_life = 10;
			render_aabb = { {1, 1, 1} };
			break;
		default:
			break;
		}

		render_aabb.Update(position);

		life = max_life;
		ticks_alive = 0;
	}

	int NPC::MaxLife() const
	{
		return max_life;
	}

	AABB& NPC::GetRenderAABB()
	{
		return render_aabb;
	}

	void NPC::Tick()
	{
		if (active) {

			render_aabb.Update(position);
			ticks_alive++;
		}
	}

	void NPC::Render(Camera& camera, glm::vec3 cameraPosition)
	{
		switch (type)
		{
		case Eon::NPCType::FISH:
			fish_renderer->Render(camera, cameraPosition, position, rotation, { 1, 1, 1 }, color, ticks_alive / 10.0f);
			break;
		default:
			break;
		}
	}

	void NPC::InitRenderers()
	{
		if (fish_renderer == nullptr) {
			fish_renderer = std::make_unique<FishRenderer>();
		}
	}
}
