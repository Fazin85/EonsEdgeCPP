#pragma once

namespace Eon
{
	struct Settings
	{
		int render_distance = 32;
		int mesh_gen_threads_count = 4;
		int maxFps = 400;
		float fov = 70;
		bool fog = true;
		bool percent_lod = true;
	};

	static inline Settings GameSettings;
}