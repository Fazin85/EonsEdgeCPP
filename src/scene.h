#pragma once

#include <string>


namespace Eon
{
	class Scene
	{
	public:
		virtual ~Scene() = default;

		virtual const char* GetName() = 0;
		virtual void Update(float dt) = 0;
		virtual void Render() = 0;
	};
}