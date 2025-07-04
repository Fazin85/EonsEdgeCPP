#pragma once

#include "render_pass.h"

namespace Eon
{
	class DepthSortedRenderPass : public RenderPass
	{
	public:
		DepthSortedRenderPass(RenderPipeline& pipeline, const std::string& name);

		void Execute(RenderState& renderState) override;
		void End(RenderState& renderState) override;

		void Submit(RenderCommandVariant& renderCommand) override;
	};
}