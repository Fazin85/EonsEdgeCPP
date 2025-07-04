#pragma once

#include "../depth_sorted_render_pass.h"

namespace Eon
{
	class TranslucentRenderPass : public DepthSortedRenderPass
	{
	public:
		explicit TranslucentRenderPass(RenderPipeline& pipeline);
		
		void Begin(RenderState& renderState) override;
	};
}