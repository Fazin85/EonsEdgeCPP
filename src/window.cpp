#include "window.h"

namespace Eon
{
	void Window::Create(int width, int height, int fps, const std::string& name)
	{
		sf::ContextSettings settings;
		settings.depthBits = 24;
		settings.majorVersion = 3;
		settings.minorVersion = 3;

		window = new sf::Window(sf::VideoMode(width, height), name, sf::Style::Close, settings);
		window->setFramerateLimit(fps);
		window->setActive(true);
	}

	sf::Window& Window::Get()
	{
		return *window;
	}
}
