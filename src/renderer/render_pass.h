#pragma once

#include <string>
#include "command/render_command_variant.h"
#include "render_pipeline.h"
#include "../camera.h"

namespace Eon
{
	class RenderPipeline;

	class RenderPass
	{
	public:
		explicit RenderPass(RenderPipeline& pipeline, const std::string& name);
		virtual ~RenderPass() = default;

		virtual void Begin(RenderState& renderState) = 0;
		virtual void Execute(RenderState& renderState) = 0;
		virtual void End(RenderState& renderState);

		virtual void Submit(RenderCommandVariant& renderCommand) = 0;

	protected:
		std::string name;
		std::vector<std::reference_wrapper<RenderCommandVariant>> render_commands;
		RenderPipeline& pipeline;
	};
}