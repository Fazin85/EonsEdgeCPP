#include "scope_timer.h"

namespace Eon
{
	ScopeTimer::ScopeTimer(const std::string name, TimeUnit unit)
	{
		this->name = name;
		this->unit = unit;
	}

	ScopeTimer::~ScopeTimer()
	{
		if (unit == TimeUnit::Milliseconds)
		{
			EON_INFO(name + ": " + std::to_string(clock.getElapsedTime().asMilliseconds()));
		}
		else if (unit == TimeUnit::Microseconds)
		{
			EON_INFO(name + ": " + std::to_string(clock.getElapsedTime().asMicroseconds()));
		}
	}
}