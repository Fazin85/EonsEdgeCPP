#pragma once

#include "gui_element.h"

namespace Eon
{
	class GuiPanel : public GuiElement
	{
	public:
		explicit GuiPanel(int x, int y, int width, int height);

		void Render(RenderPipeline& renderPipeline, RenderCommandPool& commandPool, ScaledResolution& scaledResolution) override;
	};

}