#pragma once

#include "chunk_mesh_render_command.h"
#include "mesh_render_command_colored.h"
#include <variant>

namespace Eon
{
	using RenderCommandVariant = std::variant<std::monostate, MeshRenderCommand, ChunkMeshRenderCommand, MeshRenderCommandColored>;

	RenderCommand& GetRenderCommand(RenderCommandVariant& variant);
}