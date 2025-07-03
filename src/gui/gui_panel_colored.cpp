#include "gui_panel_colored.h"
#include "../renderer/material.h"

namespace Eon
{
	GuiPanelColored::GuiPanelColored(
		int x,
		int y,
		int width,
		int height,
		float r,
		float g,
		float b) : GuiElement(x, y, width, height), r(r), g(g), b(b)
	{
	}

	void GuiPanelColored::Render(RenderPipeline& renderPipeline, RenderCommandPool& commandPool, ScaledResolution& scaledResolution)
	{
		Material mat{ AssetManager::GetAsset<Texture>("texture.white_dot").GetID(), AssetManager::GetAsset<Shader>("shader.pt_colored").GetID(), TransparencyType::Opaque };

		DrawColoredRect(
			renderPipeline,
			commandPool,
			static_cast<float>(GetX()),
			static_cast<float>(GetY()),
			static_cast<float>(GetWidth()),
			static_cast<float>(GetHeight()),
			mat,
			r,
			g,
			b);
	}

	void GuiPanelColored::SetColor(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}
}
