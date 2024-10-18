#pragma once

#include "bounding_box.h"
#include "camera.h"
#include "glm/vec3.hpp"
#include "level.h"
#include "log.h"
#include <memory>

#define Y_JUMP_VELOCITY 8.5f
#define CAMERA_HEIGHT 1.62f
#define WALKING_SPEED 0.75f
#define SPRINTING_SPEED 1.0f

namespace Eon
{
	class Player
	{
	public:
		Player(Level* level);
		void Update(float dt);
		Camera& GetCamera();
		glm::vec3 Position() const;

	private:
		void Move(glm::vec3 amount);
		void Jump();

		glm::vec3 position;
		glm::vec3 acceleration;
		glm::vec3 velocity;
		Level* level;
		BoundingBox bb;
		bool on_ground;
		float speed;

		std::unique_ptr<Camera> camera;
	};
}