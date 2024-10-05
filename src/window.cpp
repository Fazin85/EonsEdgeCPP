#include "window.h"

#include <iostream>

Window::Window(int width, int height, const std::string& title) {
  window = nullptr;
  size = glm::ivec2(width, height);

  if (!glfwInit()) {
    std::cerr << "Failed to init GLFW" << std::endl;
    return;
  }

  window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);

  if (!window) {
    glfwTerminate();
    std::cerr << "Failed to create GLFW window" << std::endl;
    return;
  }

  glfwMakeContextCurrent(window);

  gladLoadGL();
}

Window::~Window() { glfwTerminate(); }

glm::ivec2 Window::GetCursorPosition() {
  double x = 0;
  double y = 0;

  glfwGetCursorPos(window, &x, &y);

  return glm::ivec2(static_cast<int>(x), static_cast<int>(y));
}

void Window::SetCursorPosition(glm::ivec2 position) {
  glfwSetCursorPos(window, static_cast<double>(position.x),
                   static_cast<double>(position.y));
}

void Window::SwapBuffers() { glfwSwapBuffers(window); }

void Window::PollEvents() { glfwPollEvents(); }

bool Window::IsKeyPressed(int key) {
  if (glfwGetKey(window, key) == GLFW_PRESS) {
    return true;
  }

  return false;
}

bool Window::ShouldClose() { return glfwWindowShouldClose(window); }

glm::ivec2 Window::GetSize() const { return size; }

float Window::GetAspectRatio() const {
  return static_cast<float>(size.x) / static_cast<float>(size.y);
}
