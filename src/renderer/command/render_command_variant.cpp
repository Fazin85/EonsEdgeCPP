#include "render_command_variant.h"

namespace Eon
{
	RenderCommand& Eon::GetRenderCommand(RenderCommandVariant& variant)
	{
		return std::visit([](auto& cmd) -> RenderCommand&
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(cmd)>, std::monostate>)
				{
					throw std::runtime_error("Empty RenderCommandVariant");
				}
				else
				{
					return cmd;
				}
			}, variant);
	}
}