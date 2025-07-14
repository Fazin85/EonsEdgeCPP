#pragma once

#include "log.h"
#include <SFML/System.hpp>
#include <string> 

namespace Eon
{
	class ScopeTimer
	{
	public:
		enum class TimeUnit
		{
			Milliseconds,
			Microseconds
		};

		ScopeTimer(const std::string name, TimeUnit unit = TimeUnit::Milliseconds);
		~ScopeTimer();

	private:
		sf::Clock clock;
		std::string name;
		TimeUnit unit;
	};
}