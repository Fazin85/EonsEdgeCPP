#pragma once

#include "text_renderer.h"
#include "imgui_manager.h"
#include "scene_manager.h"

namespace Eon
{
	class Game
	{
	public:
		int Run();
	private:
		void Init();
		void Update(float dt);
		void Render();
		void OnExit();
		void Stop(int exitCode);

		int exitCode;
		bool stop;
		std::unique_ptr<SceneManager> scene_manager;
		std::unique_ptr<TextRenderer> text_renderer;
		std::unique_ptr<ImGuiManager> imgui_manager;
	};
}