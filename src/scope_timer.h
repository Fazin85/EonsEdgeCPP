#pragma once

#include "log.h"
#include <SFML/System.hpp>
#include <string> 

namespace Eon
{
	class ScopeTimer
	{
	public:
		ScopeTimer(const std::string name);
		~ScopeTimer();

	private:
		sf::Clock clock;
		std::string name;
	};
}