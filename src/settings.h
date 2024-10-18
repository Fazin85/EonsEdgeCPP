#pragma once

namespace Eon
{
	struct Settings
	{
		int render_distance = 12;
		bool fog = true;
	};

	static inline Settings GameSettings;
}