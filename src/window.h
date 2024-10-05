#pragma once

#include <glm/vec2.hpp>
#include <string>

// glad must be included before glfw
#include "glad/glad.h"
#include "GLFW/glfw3.h"

class Window {
 public:
  Window(int width, int height, const std::string& title);
  ~Window();
  glm::ivec2 GetCursorPosition();
  void SetCursorPosition(glm::ivec2 position);
  void SwapBuffers();
  void PollEvents();
  bool IsKeyPressed(int key);
  bool ShouldClose();
  glm::ivec2 GetSize() const;
  float GetAspectRatio() const;

 private:
  GLFWwindow* window;
  glm::ivec2 size;
};