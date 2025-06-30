#pragma once

#include "scene.h"

#include <memory>
#include <vector>

namespace Eon
{
	class SceneManager
	{
	public:
		void LoadScene(std::shared_ptr<Scene> scene);
		bool SetSceneChangedThisUpdate(bool value);

		std::shared_ptr<Scene> GetCurrentScene();

		bool SceneChangedThisUpdate() const;

	private:
		std::shared_ptr<Scene> current_scene = nullptr;
		bool scene_changed_this_update = false;
	};
}