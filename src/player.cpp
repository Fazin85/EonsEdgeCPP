#include "level.h"
#include "player.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Eon
{
	constexpr glm::vec3 dimensions{ 0.6f, 1.8f, 0.6f };

	Player::Player(Level* level) : bb(glm::vec3(0), glm::vec3(0))
	{
		this->level = level;
		camera = std::make_unique<Camera>(90, 8);
		position = glm::vec3(256, 200, 256);
		acceleration = glm::vec3(0, 0, 0);
		velocity = glm::vec3(0, 0, 0);
		glm::vec3 halfDim(dimensions.x / 2, dimensions.y / 2, dimensions.z / 2);
		on_ground = false;
		bb = BoundingBox(position - halfDim, position + halfDim);
		speed = WALKING_SPEED;
	}

	void Player::Update(float dt)
	{
		camera->Update(dt);

		float realSpeed = speed;

		if (!on_ground)
		{
			realSpeed *= 0.5f;
		}

		if (Window::Get().hasFocus())
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			{
				acceleration.x -= std::cosf(glm::radians(camera->Yaw() + 180.0f)) * realSpeed;
				acceleration.z -= std::sinf(glm::radians(camera->Yaw() + 180.0f)) * realSpeed;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				acceleration.x += std::cosf(glm::radians(camera->Yaw() + 180.0f)) * realSpeed;
				acceleration.z += std::sinf(glm::radians(camera->Yaw() + 180.0f)) * realSpeed;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				acceleration.x -= std::cosf(glm::radians(camera->Yaw() + 90.0f)) * realSpeed;
				acceleration.z -= std::sinf(glm::radians(camera->Yaw() + 90.0f)) * realSpeed;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			{
				acceleration.x += std::cosf(glm::radians(camera->Yaw() + 90.0f)) * realSpeed;
				acceleration.z += std::sinf(glm::radians(camera->Yaw() + 90.0f)) * realSpeed;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
			{
				Jump();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
			{
				speed = SPRINTING_SPEED;
			}
			else
			{
				speed = WALKING_SPEED;
			}
		}

		velocity += acceleration;
		acceleration = glm::vec3(0, 0, 0);

		if (!on_ground)
		{
			velocity.y -= 32 * dt;
		}

		on_ground = false;

		velocity.x *= 0.91f;
		velocity.z *= 0.91f;

		if (on_ground)
		{
			velocity.x *= 0.6f;
			velocity.z *= 0.6f;
		}

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

		on_ground = originalVelocity.y != amount.y && originalVelocity.y < 0.0f;

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

		position.x = (bb.min.x + bb.max.x) / 2.0f;
		position.y = bb.min.y + CAMERA_HEIGHT;
		position.z = (bb.min.z + bb.max.z) / 2.0f;
	}

	void Player::Jump()
	{
		if (on_ground)
		{
			acceleration.y += Y_JUMP_VELOCITY;
		}
	}
}