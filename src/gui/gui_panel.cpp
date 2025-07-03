#include "gui_panel.h"

namespace Eon
{
	GuiPanel::GuiPanel(int x, int y, int width, int height) : GuiElement(x, y, width, height)
	{
	}

	void GuiPanel::Render(RenderPipeline& renderPipeline, RenderCommandPool& commandPool, ScaledResolution& scaledResolution)
	{
		//DrawRect(renderPipeline, commandPool, GetX(), GetY(), GetWidth(), GetHeight(), );
	}
}
