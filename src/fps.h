#pragma once

#include "SFML/System.hpp"

namespace Eon
{
	class FPS
	{
	public:
		FPS() : frame(0), fps(0) {}

		const unsigned int Get() const { return fps; }

		void Update()
		{
			if (clock.getElapsedTime().asSeconds() >= 1.f)
			{
				fps = frame;
				frame = 0;
				clock.restart();
			}

			++frame;
		}

	private:
		unsigned int frame;
		unsigned int fps;
		sf::Clock clock;
	};
}