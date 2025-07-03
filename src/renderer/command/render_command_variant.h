#pragma once

#include "chunk_mesh_render_command.h"
#include <variant>

namespace Eon
{
	using RenderCommandVariant = std::variant<std::monostate, MeshRenderCommand, ChunkMeshRenderCommand>;

	RenderCommand& GetRenderCommand(RenderCommandVariant& variant);
}