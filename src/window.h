#pragma once

#include <glm/vec2.hpp>
#include <string>

// glad must be included before glfw
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Eon
{
	class Window
	{
	public:
		static void Create(int width, int height, const std::string& title);
		static void Destroy();
		static void SwapBuffers();
		static void PollEvents();
		static bool ShouldClose();
		static float GetAspectRatio();
		static void SetCursorPosition(glm::ivec2 position);
		static glm::ivec2 GetCursorPosition();
		static glm::ivec2 GetSize();
		static bool IsKeyPressed(int key);
	private:
		static inline GLFWwindow* window;
		static inline glm::ivec2 size;
	};
}  // namespace Eon