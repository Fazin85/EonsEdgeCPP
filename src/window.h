#pragma once

#include <SFML/Window.hpp>
#include <string>

namespace Eon
{
	class Window
	{
	public:
		static void Create(int width, int height, int fps, const std::string& name);
		static sf::Window& Get();

	private:
		static inline sf::Window* window;
	};
}