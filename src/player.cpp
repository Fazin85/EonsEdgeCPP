#include "level.h"
#include "player.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Eon
{
	glm::vec3 dimensions{ 0.6f, 1.8f, 0.6f };

	Player::Player(Level* level) : bb(glm::vec3(0), glm::vec3(0))
	{
		this->level = level;
		camera = std::make_unique<Camera>(70, 0.1);
		position = glm::vec3(0, 0, 0);
		acceleration = glm::vec3(0, 0, 0);
		velocity = glm::vec3(0, 0, 0);
		glm::vec3 halfDim(dimensions.x / 2, dimensions.y / 2, dimensions.z / 2);

		bb = BoundingBox(position - halfDim, position + halfDim);
	}

	void Player::Update(float dt)
	{
		camera->Update(dt);

		constexpr float speed = 0.08f;

		if (Window::Get().hasFocus())
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			{
				acceleration.x -= std::cosf(glm::radians(camera->Yaw() + 180.0f)) * speed;
				acceleration.z -= std::sinf(glm::radians(camera->Yaw() + 180.0f)) * speed;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				acceleration.x += std::cosf(glm::radians(camera->Yaw() + 180.0f)) * speed;
				acceleration.z += std::sinf(glm::radians(camera->Yaw() + 180.0f)) * speed;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				acceleration.x -= std::cosf(glm::radians(camera->Yaw() + 90.0f)) * speed;
				acceleration.z -= std::sinf(glm::radians(camera->Yaw() + 90.0f)) * speed;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			{
				acceleration.x += std::cosf(glm::radians(camera->Yaw() + 90.0f)) * speed;
				acceleration.z += std::sinf(glm::radians(camera->Yaw() + 90.0f)) * speed;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
			{
				acceleration.y += speed;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
			{
				acceleration.y -= speed;
			}
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
		glm::vec3 originalVelocity = amount;
		std::vector<BoundingBox> bbs = level->GetBlockAABBs(bb.Expand(amount));

		for (const auto& boundingBox : bbs)
		{
			amount.y = boundingBox.ClipCollideY(bb, amount.y);
		}

		bb.Move(glm::vec3(0.0f, amount.y, 0.0f));

		for (const auto& boundingBox : bbs)
		{
			amount.x = boundingBox.ClipCollideX(bb, amount.x);
		}

		bb.Move(glm::vec3(amount.x, 0.0f, 0.0f));

		for (const auto& boundingBox : bbs)
		{
			amount.z = boundingBox.ClipCollideZ(bb, amount.z);
		}

		bb.Move(glm::vec3(0.0f, 0.0f, amount.z));

		if (originalVelocity.x != amount.x)
		{
			velocity.x = 0.0f;
		}

		if (originalVelocity.y != amount.y)
		{
			velocity.y = 0.0f;
		}

		if (originalVelocity.z != amount.z)
		{
			velocity.z = 0.0f;
		}

		constexpr float cameraHeight = 1.62f;

		position.x = (bb.min.x + bb.max.x) / 2.0f;
		position.y = bb.min.y + cameraHeight;
		position.z = (bb.min.z + bb.max.z) / 2.0f;
	}
}