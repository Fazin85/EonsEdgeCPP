#pragma once

#include "bounding_box.h"
#include "camera.h"
#include "glm/vec3.hpp"
#include "level.h"
#include "log.h"
#include <memory>

#define Y_JUMP_VELOCITY 8.5
#define CAMERA_HEIGHT 1.62
#define WALKING_SPEED 2.0
#define SPRINTING_SPEED 4.75

namespace Eon
{
	class Player
	{
	public:
		explicit Player(Level* level);
		void Update(float dt);
		Camera& GetCamera();
		glm::dvec3 Position() const;

	private:
		void Move(glm::dvec3 amount);
		void Jump(float dt);

		glm::dvec3 position;
		glm::dvec3 acceleration;
		glm::dvec3 velocity;
		Level* level;
		BoundingBox bb;
		bool on_ground;
		double speed;

		std::unique_ptr<Camera> camera;
	};
}