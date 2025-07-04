#pragma once

#include "mesh_render_command.h"
#include "mesh_render_command_colored.h"
#include <variant>

namespace Eon
{
	using RenderCommandVariant = std::variant<std::monostate, MeshRenderCommand, MeshRenderCommandColored>;

	RenderCommand& GetRenderCommand(RenderCommandVariant& variant);
}