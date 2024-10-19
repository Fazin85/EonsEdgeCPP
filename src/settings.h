#pragma once

namespace Eon
{
	struct Settings
	{
		int render_distance = 16;
		bool fog = true;
	};

	static inline Settings GameSettings;
}