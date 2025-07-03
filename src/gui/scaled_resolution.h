#pragma once

#include <glm/glm.hpp>

namespace Eon
{
	class ScaledResolution
	{
	public:
		ScaledResolution(int displayWidth, int displayHeight, int customScale = 0);

		glm::mat4 CreateProjectionMatrix() const;

	private:
		int scaled_width;
		int scaled_height;
		int scale_factor;
		double scale_factor_x;
		double scale_factor_y;
		int display_width;
		int display_height;
	};
}