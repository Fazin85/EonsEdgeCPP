#pragma once

namespace Eon
{
	class GameTime
	{
	public:
		static void Update(float dt);
		static double GetElapsedTime();

	private:
		static double elapsed_time;
	};
}