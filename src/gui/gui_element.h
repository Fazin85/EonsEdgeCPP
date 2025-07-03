#pragma once

#include <vector>
#include <memory>

namespace Eon
{
	class RenderPipeline;
	class RenderCommandPool;
	class ScaledResolution;
	class Mesh;
	class Material;

	class GuiElement
	{
	public:
		explicit GuiElement(int x, int y, int width, int height);
		virtual ~GuiElement() = default;

		virtual void Update();
		virtual void Render(RenderPipeline& renderPipeline, RenderCommandPool& commandPool, ScaledResolution& scaledResolution);

		void SetEnabled(bool enabled);
		void SetX(int x);
		void SetY(int y);
		void SetWidth(int width);
		void SetHeight(int height);
		void SetPosition(int x, int y);
		void SetSize(int width, int height);

		bool IsEnabled() const;
		int GetX() const;
		int GetY() const;
		int GetWidth() const;
		int GetHeight() const;
		bool HasParent() const;
		GuiElement* GetParent() const;
		bool HasChildren() const;
		std::vector<GuiElement*>& GetChildren();
		static Mesh& GetQuadMesh();

	protected:
		static void DrawRect(
			RenderPipeline& renderPipeline,
			RenderCommandPool& commandPool,
			float x,
			float y,
			float width,
			float height,
			Material& material);

	private:
		static std::unique_ptr<Mesh> quad_mesh;

		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
		bool enabled = true;
		GuiElement* parent = nullptr;
		std::vector<GuiElement*> children;
	};
}