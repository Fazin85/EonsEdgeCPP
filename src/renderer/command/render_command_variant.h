#pragma once

#include "mesh_render_command.h"
#include <variant>

namespace Eon
{
	using RenderCommandVariant = std::variant<MeshRenderCommand>;

	RenderCommand& GetRenderCommand(RenderCommandVariant& variant);
}