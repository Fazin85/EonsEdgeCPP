#include "scaled_resolution.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Eon
{
	ScaledResolution::ScaledResolution(int displayWidth, int displayHeight, int customScale) : display_width(displayWidth), display_height(displayHeight)
	{
		if (customScale == 0)
		{
			// Auto-calculate scale factor
			scale_factor = 1;
			while (scale_factor < displayWidth && scale_factor < displayHeight &&
				displayWidth / (scale_factor + 1) >= 320 &&
				displayHeight / (scale_factor + 1) >= 240)
			{
				scale_factor++;
			}
		}
		else
		{
			scale_factor = customScale;
		}

		scaled_width = displayWidth / scale_factor;
		scaled_height = displayHeight / scale_factor;
		scale_factor_x = static_cast<double>(displayWidth) / scaled_width;
		scale_factor_y = static_cast<double>(displayHeight) / scaled_height;
	}

	glm::mat4 ScaledResolution::CreateProjectionMatrix() const
	{
		return glm::ortho(0.0f, static_cast<float>(scaled_width),
			static_cast<float>(scaled_height), 0.0f,
			-1.0f, 1.0f);
	}
}
