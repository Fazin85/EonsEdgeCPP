#include "game_time.h"

namespace Eon
{
	double GameTime::elapsed_time = 0.0;

	void GameTime::Update(float dt)
	{
		elapsed_time += dt;
	}

	double Eon::GameTime::GetElapsedTime()
	{
		return elapsed_time;
	}
}