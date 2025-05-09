#pragma once

#include "billboard_sprite.h"
#include "level.h"
#include "level_renderer.h"
#include "plane_mesh.h"
#include "player.h"
#include "skybox.h"
#include "text_renderer.h"

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
		std::unique_ptr<Player> player;
		std::unique_ptr<Level> level;
		std::unique_ptr<LevelRenderer> level_renderer;
		std::unique_ptr<BillboardSprite> sprite;
		std::unique_ptr<Skybox> skybox;
		std::unique_ptr<TextRenderer> text_renderer;
	};
}