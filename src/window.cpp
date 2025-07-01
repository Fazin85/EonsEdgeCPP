#include "window.h"

namespace Eon
{
	void Window::Create(int width, int height, int fps, const std::string& name, bool fullscreen)
	{
		if (window == nullptr)
		{
			window = new sf::Window();
		}

		sf::ContextSettings settings;
		settings.depthBits = 24;
		settings.majorVersion = 3;
		settings.minorVersion = 3;
		settings.attributeFlags = sf::ContextSettings::Debug;

		Window::fullscreen = fullscreen;

		window->create(sf::VideoMode(width, height), name, Window::fullscreen ? sf::Style::Fullscreen : sf::Style::Close, settings);
		window->setFramerateLimit(fps);
		window->setActive(true);
	}

	void Window::Destroy()
	{
		window->close();
	}

	sf::Window& Window::Get()
	{
		return *window;
	}

	float Window::Aspect()
	{
		return static_cast<float>(window->getSize().x) / static_cast<float>(window->getSize().y);
	}
	bool Window::Fullscreen()
	{
		return fullscreen;
	}
}
