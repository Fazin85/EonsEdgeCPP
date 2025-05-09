#pragma once

#include <string>


namespace Eon
{
	class Scene
	{
	public:
		virtual ~Scene() = 0;

		virtual std::string& GetName() = 0;
		virtual void OnEnter() = 0;
		virtual void OnClose() = 0;
		virtual void Update(float dt) = 0;
		virtual void Render() = 0;

	private:
		std::string name;
	};
}