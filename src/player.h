#pragma once

#include "camera.h"
#include "glm/vec3.hpp"
#include "log.h"
#include <memory>

namespace Eon
{
	class Player
	{
	public:
		Player();
		void Update(float dt);
		Camera& GetCamera();
		glm::vec3 Position() const;

	private:
		void Move(glm::vec3 amount);

		glm::vec3 position;
		glm::vec3 acceleration;
		glm::vec3 velocity;
		std::unique_ptr<Camera> camera;
	};
}