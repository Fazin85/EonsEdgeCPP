#pragma once

#include "command/render_command_variant.h"

namespace Eon
{
	class RenderCommandPool
	{
	public:
		RenderCommandPool()
		{
			commands.reserve(10000);
		}

		template<typename T, typename... Args>
		RenderCommandVariant& CreateCommand(Args&&... args)
		{
			static_assert(std::is_base_of_v<RenderCommand, T>, "T must inherit from RenderCommand");

			if (currentIndex < commands.size())
			{
				commands[currentIndex].emplace<T>(std::forward<Args>(args)...);
				currentIndex++;
				return commands[currentIndex - 1];
			}
			else
			{
				commands.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
				currentIndex++;
				return commands.back();
			}
		}

		void Reset()
		{
			currentIndex = 0;
		}

		size_t Size() const { return currentIndex; }
		size_t Capacity() const { return commands.size(); }

	private:
		std::vector<RenderCommandVariant> commands;
		size_t currentIndex = 0;
	};
}