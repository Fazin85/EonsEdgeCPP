#include "scene_manager.h"
#include "log.h"

namespace Eon
{
	void SceneManager::LoadScene(std::shared_ptr<Scene> scene)
	{
		current_scene = scene;
		SetSceneChangedThisUpdate(true);
	}

	bool SceneManager::SetSceneChangedThisUpdate(bool value)
	{
		return scene_changed_this_update = value;
	}

	std::shared_ptr<Scene> SceneManager::GetCurrentScene()
	{
		if (current_scene)
		{
			return current_scene;
		}

		throw std::exception("No Current scene");
	}

	bool SceneManager::SceneChangedThisUpdate() const
	{
		return scene_changed_this_update;
	}
}