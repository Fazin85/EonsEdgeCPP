#pragma once

#include "../render_state.h"
#include "../material.h"

namespace Eon
{
	class RenderPipeline;

	class RenderCommand
	{
	public:
		virtual ~RenderCommand() = default;

		virtual void Execute(RenderState& renderState) = 0;
		virtual float GetDepth() const = 0;
		virtual Material GetMaterial() const = 0;
	};
}