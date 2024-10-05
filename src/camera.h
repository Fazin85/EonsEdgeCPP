#pragma once

#include "window.h"

#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Camera {
 public:
  Camera(float fov, float sensitivity, Window& window);
  void CalculateViewMatrix(glm::vec3 position);
  glm::mat4 GetViewMatrix() const;
  void Update(float dt);

 private:
  void CalculateProjectionMatrix();

  Window& window;
  glm::mat4 view_matrix;
  glm::mat4 proj_matrix;
  glm::vec3 up;
  glm::vec3 front;
  glm::vec3 right;
  glm::ivec2 last_mouse_pos;
  glm::vec2 clipping_planes;
  float pitch;
  float yaw;
  float fov;
  float sensitivity;
};