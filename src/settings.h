#pragma once

namespace Eon
{
	struct Settings
	{
		int render_distance = 32;
		bool fog = false;
	};

	static inline Settings GameSettings;
}