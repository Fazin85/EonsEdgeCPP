#include <glm/gtc/matrix_transform.hpp>
#include "player.h"

namespace Eon
{
	Player::Player()
	{
		camera = std::make_unique<Camera>(70, 8);
		position = glm::vec3(0, 0, 0);
		acceleration = glm::vec3(0, 0, 0);
		velocity = glm::vec3(0, 0, 0);
	}

	void Player::Update(float dt)
	{
		camera->Update(dt);

		constexpr float speed = 1.8f;

		if (Window::IsKeyPressed(GLFW_KEY_W))
		{
			acceleration.x -= std::cosf(glm::radians(camera->Yaw() + 180.0f)) * speed;
			acceleration.z -= std::sinf(glm::radians(camera->Yaw() + 180.0f)) * speed;
		}

		if (Window::IsKeyPressed(GLFW_KEY_S))
		{
			acceleration.x += std::cosf(glm::radians(camera->Yaw() + 180.0f)) * speed;
			acceleration.z += std::sinf(glm::radians(camera->Yaw() + 180.0f)) * speed;
		}

		if (Window::IsKeyPressed(GLFW_KEY_A))
		{
			acceleration.x -= std::cosf(glm::radians(camera->Yaw() + 90.0f)) * speed;
			acceleration.z -= std::sinf(glm::radians(camera->Yaw() + 90.0f)) * speed;
		}

		if (Window::IsKeyPressed(GLFW_KEY_D))
		{
			acceleration.x += std::cosf(glm::radians(camera->Yaw() + 90.0f)) * speed;
			acceleration.z += std::sinf(glm::radians(camera->Yaw() + 90.0f)) * speed;
		}

		if (Window::IsKeyPressed(GLFW_KEY_SPACE))
		{
			acceleration.y += speed;
		}

		if (Window::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
		{
			acceleration.y -= speed;
		}

		velocity += acceleration;
		acceleration = glm::vec3(0, 0, 0);

		velocity.x *= 0.91f;
		velocity.z *= 0.91f;
		velocity.y *= 0.91f;

		if (std::abs(velocity.x) <= 0.0001f)
		{
			velocity.x = 0;
		}

		if (std::abs(velocity.z) <= 0.0001f)
		{
			velocity.z = 0;
		}

		Move(velocity * dt);
	}

	Camera& Player::GetCamera()
	{
		return *camera;
	}

	glm::vec3 Player::Position() const
	{
		return position;
	}

	void Player::Move(glm::vec3 amount)
	{
		position += amount;
	}
}