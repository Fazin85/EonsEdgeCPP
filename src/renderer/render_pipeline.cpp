#include "render_pipeline.h"

namespace Eon
{
	void RenderPipeline::RenderStats::Reset()
	{
		draw_calls = 0;
		shader_binds = 0;
		texture_binds = 0;
		time_ms = 0;
	}
}
