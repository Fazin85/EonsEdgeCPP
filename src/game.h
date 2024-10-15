#pragma once

#include "billboard_sprite.h"
#include "level.h"
#include "level_renderer.h"
#include "player.h"

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
	};
}