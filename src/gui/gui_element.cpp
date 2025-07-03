#include "gui_element.h"
#include "../renderer/render_pipeline.h"
#include "../renderer/render_command_pool.h"
#include "scaled_resolution.h"
#include "../mesh.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Eon
{
	std::unique_ptr<Mesh> GuiElement::quad_mesh = nullptr;

	GuiElement::GuiElement(int x, int y, int width, int height) : x(x), y(y), width(width), height(height)
	{
	}

	void GuiElement::Update()
	{
		for (auto& child : children)
		{
			if (child)
			{
				child->Update();
			}
		}
	}

	void GuiElement::Render(RenderPipeline& renderPipeline, RenderCommandPool& commandPool, ScaledResolution& scaledResolution)
	{
		for (auto& child : children)
		{
			if (child)
			{
				child->Render(renderPipeline, commandPool, scaledResolution);
			}
		}
	}

	void GuiElement::SetEnabled(bool enabled)
	{
		this->enabled = enabled;
	}

	void GuiElement::SetX(int x)
	{
		this->x = x;
	}

	void GuiElement::SetY(int y)
	{
		this->y = y;
	}

	void GuiElement::SetWidth(int width)
	{
		this->width = width;
	}

	void GuiElement::SetHeight(int height)
	{
		this->height = height;
	}

	void GuiElement::SetPosition(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	void GuiElement::SetSize(int width, int height)
	{
		this->width = width;
		this->height = height;
	}

	bool GuiElement::IsEnabled() const
	{
		return enabled;
	}

	int GuiElement::GetX() const
	{
		return x;
	}

	int GuiElement::GetY() const
	{
		return y;
	}

	int GuiElement::GetWidth() const
	{
		return width;
	}

	int GuiElement::GetHeight() const
	{
		return height;
	}

	bool GuiElement::HasParent() const
	{
		return parent;
	}

	GuiElement* GuiElement::GetParent() const
	{
		return parent;
	}

	bool GuiElement::HasChildren() const
	{
		return !children.empty();
	}

	std::vector<GuiElement*>& GuiElement::GetChildren()
	{
		return children;
	}

	Mesh& GuiElement::GetQuadMesh()
	{
		if (quad_mesh == nullptr)
		{
			std::vector<glm::vec3, PoolAllocator<glm::vec3>> positions = {
				{
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(1.0f, 1.0f, 0.0f)
				},
				*PoolAllocators::GetInstance().vec3_allocator
			};

			std::vector<glm::vec2, PoolAllocator<glm::vec2>> uvs = {
				{
				glm::vec2(0.0f, 1.0f),
				glm::vec2(0.0f, 0.0f),
				glm::vec2(1.0f, 0.0f),
				glm::vec2(0.0f, 1.0f),
				glm::vec2(1.0f, 0.0f),
				glm::vec2(1.0f, 1.0f)
				},
				*PoolAllocators::GetInstance().vec2_allocator
			};

			quad_mesh = std::make_unique<PositionTextureMesh>(std::move(positions), std::move(uvs));
			quad_mesh->Setup();
		}

		return *quad_mesh;
	}

	void GuiElement::DrawTexturedRect(
		RenderPipeline& renderPipeline,
		RenderCommandPool& commandPool,
		float x,
		float y,
		float width,
		float height,
		Material& material)
	{
		auto model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(x, y, 0.0f));
		model = glm::scale(model, glm::vec3(width, height, 1.0f));

		auto& renderCommand = commandPool.CreateCommand<MeshRenderCommand>(GetQuadMesh(), model, 0.0f, material);

		renderPipeline.Submit(renderCommand);
	}

	void GuiElement::DrawColoredRect(
		RenderPipeline& renderPipeline,
		RenderCommandPool& commandPool,
		float x,
		float y,
		float width,
		float height,
		Material& material,
		float r,
		float g,
		float b)
	{
		auto model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(x, y, 0.0f));
		model = glm::scale(model, glm::vec3(width, height, 1.0f));

		auto& renderCommand = commandPool.CreateCommand<MeshRenderCommandColored>(GetQuadMesh(), model, 0.0f, material, r, g, b);

		renderPipeline.Submit(renderCommand);
	}
}