#include "scene_manager.h"
#include "log.h"

namespace Eon
{
	std::vector<std::shared_ptr<Scene>> SceneManager::scenes;
	size_t SceneManager::current_scene_index = -1;

	void SceneManager::RegisterScene(std::shared_ptr<Scene> scene)
	{
		for (const std::shared_ptr<Scene>& s : scenes)
		{
			if (s->GetName() == scene->GetName())
			{
				std::stringstream ss;
				ss << "Cannot have multiple scenes with the same name: " << scene->GetName();

				EON_WARN(ss.str());
			}

			scenes.emplace_back(scene);
		}
	}

	void SceneManager::LoadSceneByName(const std::string& name)
	{
		for (size_t i = 0; i < scenes.size(); i++)
		{
			if (scenes[i]->GetName() == name)
			{
				if (current_scene_index > 0 && current_scene_index <= scenes.size())
				{
					auto& currentScene = scenes[current_scene_index];

					currentScene->OnClose();
				}

				current_scene_index = i;
				auto& currentScene = scenes[current_scene_index];
				currentScene->OnEnter();
				return;
			}
		}
	}

	void SceneManager::LoadScene(std::shared_ptr<Scene> scene)
	{
		for (size_t i = 0; i < scenes.size(); i++)
		{
			if (scenes[i]->GetName() == scene->GetName())
			{
				if (current_scene_index > 0 && current_scene_index <= scenes.size())
				{
					auto& currentScene = scenes[current_scene_index];

					currentScene->OnClose();
				}

				current_scene_index = i;
				auto& currentScene = scenes[current_scene_index];
				currentScene->OnEnter();
				return;
			}
		}
	}

}