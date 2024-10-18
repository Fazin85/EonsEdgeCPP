#pragma once

#include <SFML/Window.hpp>
#include <string>

namespace Eon
{
	class Window
	{
	public:
		static void Create(int width, int height, int fps, const std::string& name, bool fullscreen);
		static void Destroy();
		static sf::Window& Get();
		static float Aspect();
		static bool Fullscreen();

	private:
		static inline bool fullscreen;
		static inline sf::Window* window = nullptr;
	};
}