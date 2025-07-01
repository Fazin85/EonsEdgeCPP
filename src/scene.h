#pragma once

#include <string>

namespace Eon
{
	class Scene
	{
	public:
		virtual ~Scene() = default;

		virtual std::string GetName() const = 0;
		virtual void Update(float dt) = 0;
		virtual void Render() = 0;
	};
}