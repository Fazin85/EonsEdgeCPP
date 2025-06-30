#include "render_command_variant.h"

namespace Eon
{
	RenderCommand& Eon::GetRenderCommand(RenderCommandVariant& variant)
	{
		return std::visit([](auto& cmd) -> RenderCommand&
			{
				return cmd;
			}, variant);
	}
}