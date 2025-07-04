#pragma once

#include "../render_pass.h"

namespace Eon 
{
	class RenderPassSSR : public RenderPass
	{
	public:
		explicit RenderPassSSR(RenderPipeline& pipeline);

		void Execute(RenderState& renderState) override;
		void End(RenderState& renderState) override;

	private:
		std::unique_ptr<PositionTextureMesh> fullscreen_quad = nullptr;
	};
}