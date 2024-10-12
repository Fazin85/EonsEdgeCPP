#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "window.h"

namespace Eon
{
	class Camera
	{
	public:
		Camera(float fov, float sensitivity);
		void CalculateViewMatrix(glm::vec3 position);
		glm::mat4 ViewMatrix() const;
		glm::mat4 ProjectionMatrix() const;
		void Update(float dt);
		float Yaw() const;

	private:
		void CalculateProjectionMatrix();

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
}  // namespace Eon