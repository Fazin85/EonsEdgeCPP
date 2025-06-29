#pragma once

#include <string>
#include "render_command.h"
#include "../camera.h"

namespace Eon
{
	class RenderPass
	{
	public:
		explicit RenderPass(const std::string& name);
		virtual ~RenderPass() = default;

		virtual void Begin(RenderState& renderState) = 0;
		virtual void Execute(RenderState& renderState, Camera& camera, glm::vec3 cameraPosition) = 0;
		virtual void End(RenderState& renderState);

		virtual void Submit(std::unique_ptr<RenderCommand>& renderCommand) = 0;

	protected:
		std::string name;
		std::vector<std::unique_ptr<RenderCommand>> render_commands;
	};
}