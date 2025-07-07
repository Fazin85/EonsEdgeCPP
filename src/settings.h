#pragma once

namespace Eon
{
	struct Settings
	{
		int render_distance = 24;
		int mesh_gen_threads_count = 2;
		int max_fps = 200;
		float fov = 70;
		bool fog = true;
		bool percent_lod = true;
		bool lod = false;
	};

	static inline Settings GameSettings;
}