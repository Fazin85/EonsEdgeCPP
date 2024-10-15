#pragma once

#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <cmath>

namespace Eon
{
	Camera::Camera(float fov, float sensitivity)
	{
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
		clipping_planes[1] = 2000.0f;

		last_mouse_pos.x = sf::Mouse::getPosition().x;
		last_mouse_pos.y = sf::Mouse::getPosition().y;
		CalculateProjectionMatrix();
	}

	void Camera::CalculateViewMatrix(glm::vec3 position)
	{
		view_matrix = glm::lookAt(position, position + front, up);
	}

	glm::mat4* Camera::ViewMatrix() { return &view_matrix; }

	glm::mat4* Camera::ProjectionMatrix()
	{
		return &proj_matrix;
	}

	void Camera::Update(float dt)
	{
		if (!Window::Get().hasFocus())
		{
			return;
		}

		auto change = sf::Mouse::getPosition(Window::Get()) - last_mouse_pos;

		sf::Mouse::setPosition(sf::Vector2i(Window::Get().getSize().x / 2, Window::Get().getSize().y / 2), Window::Get());
		last_mouse_pos = sf::Mouse::getPosition(Window::Get());

		yaw += change.x * sensitivity * dt;
		pitch -= change.y * sensitivity * dt;

		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		else if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}

		if (yaw > 360)
		{
			yaw = 0;
		}
		else if (yaw < 0)
		{
			yaw = 360;
		}

		front[0] = std::cosf(glm::radians(pitch)) * std::cosf(glm::radians(yaw));
		front[1] = std::sinf(glm::radians(pitch));
		front[2] = std::cosf(glm::radians(pitch)) * std::sinf(glm::radians(yaw));

		front = glm::normalize(front);

		right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));

		up = glm::normalize(glm::cross(right, front));
	}

	float Camera::Yaw() const
	{
		return yaw;
	}

	Frustum& Camera::GetFrustum()
	{
		frustum.Update(proj_matrix * view_matrix);

		return frustum;
	}

	glm::vec2 Camera::ClippingPlanes() const
	{
		return clipping_planes;
	}

	float Camera::Fov() const
	{
		return fov;
	}

	void Camera::CalculateProjectionMatrix()
	{
		float aspect = static_cast<float>(Window::Get().getSize().x) / static_cast<float>(Window::Get().getSize().y);

		proj_matrix = glm::perspective(glm::radians(fov), aspect,
			clipping_planes[0], clipping_planes[1]);
	}
}  // namespace Eon
