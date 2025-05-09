#pragma once

#include "scene.h"

#include <memory>
#include <vector>

namespace Eon
{
	class SceneManager
	{
	public:
		static void RegisterScene(std::shared_ptr<Scene> scene);
		static void LoadSceneByName(const std::string& name);
		static void LoadScene(std::shared_ptr<Scene> scene);

	private:
		static std::vector<std::shared_ptr<Scene>> scenes;
		static size_t current_scene_index;
	};
}