#pragma once

namespace Eon
{
	struct Settings
	{
		int render_distance = 32;
		int mesh_gen_threads_count = 4;
		int maxFps = 160;
		bool fog = true;
		bool percent_lod = false;
	};

	static inline Settings GameSettings;
}