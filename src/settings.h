#pragma once

namespace Eon
{
	struct Settings
	{
		int render_distance = 6;
		int mesh_gen_threads_count = 2;
		int maxFps = 400;
		float fov = 70;
		bool fog = true;
		bool percent_lod = true;
		bool lod = false;
	};

	static inline Settings GameSettings;
}