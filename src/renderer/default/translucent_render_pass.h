#pragma once

#include "../depth_sorted_render_pass.h"

namespace Eon
{
	class TranslucentRenderPass : public DepthSortedRenderPass
	{
	public:
		explicit TranslucentRenderPass(RenderPipeline& pipeline, Framebuffer& framebuffer);
		
		void Begin(RenderState& renderState) override;
	};
}