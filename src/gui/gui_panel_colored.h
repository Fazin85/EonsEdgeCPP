#pragma once

#include "gui_element.h"

namespace Eon
{
	class GuiPanelColored : public GuiElement
	{
	public:
		explicit GuiPanelColored(
			int x, 
			int y, 
			int width, 
			int height, 
			float r, 
			float g, 
			float b);

		void Render(RenderPipeline& renderPipeline, RenderCommandPool& commandPool, ScaledResolution& scaledResolution) override;

		void SetColor(float r, float g, float b);

	private:
		float r;
		float g;
		float b;
	};

}