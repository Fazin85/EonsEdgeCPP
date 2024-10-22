#include "scope_timer.h"

namespace Eon
{
	ScopeTimer::ScopeTimer(const std::string name)
	{
		this->name = name;
	}

	ScopeTimer::~ScopeTimer()
	{
		EON_INFO(name + ": " + std::to_string(clock.getElapsedTime().asMilliseconds()));
	}
}