#pragma once

#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace Eon {
	Camera::Camera(float fov, float sensitivity, Window& win) : window(win) {
		up[0] = 0;
		up[1] = 1;
		up[2] = 0;

		front[0] = 0;
		front[1] = 0;
		front[2] = -1;

		right[0] = -1;
		right[1] = 0;
		right[2] = 0;

		this->fov = fov;
		this->sensitivity = sensitivity;
		yaw = -90.0f;

		clipping_planes[0] = 0.01f;
		clipping_planes[1] = 1000.0f;

		last_mouse_pos = window.GetCursorPosition();
		CalculateProjectionMatrix();
	}

	void Camera::CalculateViewMatrix(glm::vec3 position) {
		view_matrix = glm::lookAt(position, position + front, up);
	}

	glm::mat4 Camera::GetViewMatrix() const { return view_matrix; }

	void Camera::Update(float dt) {
		glm::ivec2 change = window.GetCursorPosition();

		change -= last_mouse_pos;

		window.SetCursorPosition(window.GetSize() / 2);

		last_mouse_pos = window.GetCursorPosition();

		yaw += static_cast<float>(change.x) * sensitivity * dt;
		pitch -= static_cast<float>(change.y) * sensitivity * dt;

		if (pitch > 89.0f) {
			pitch = 89.0f;
		}

		if (pitch < -89.0f) {
			pitch = -89.0f;
		}

		if (yaw > 360) {
			yaw = 0;
		}

		if (pitch > 360) {
			pitch = 0;
		}

		front[0] = cosf(glm::radians(pitch)) * cosf(glm::radians(yaw));
		front[1] = sinf(glm::radians(pitch));
		front[2] = cosf(glm::radians(pitch)) * sinf(glm::radians(yaw));

		front = glm::normalize(front);

		glm::vec3 unitY{ 0 };
		unitY[1] = 1;
		right = glm::normalize(glm::cross(front, unitY));

		up = glm::normalize(glm::cross(right, front));
	}

	void Camera::CalculateProjectionMatrix() {
		proj_matrix = glm::perspective(glm::radians(fov), window.GetAspectRatio(),
			clipping_planes[0], clipping_planes[1]);
	}
}  // namespace Eon
