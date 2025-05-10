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
		std::shared_ptr<Scene> GetCurrentScene();

	private:
		std::shared_ptr<Scene> current_scene = nullptr;
	};
}