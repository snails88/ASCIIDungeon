#include "Timer.h"

namespace Craft 
{
	Timer::Timer(float targetTime)
		: elapsedTime(0.f), targetTime(targetTime)
	{
	}

	void Timer::Tick(float deltaTime)
	{
		elapsedTime += deltaTime;

		elapsedTime = elapsedTime >= targetTime ? targetTime : elapsedTime;
	}

	void Timer::Reset()
	{
		elapsedTime = 0.f;
	}

	void Timer::SetTargetTime(float targetTime)
	{
		this->targetTime = targetTime;
	}

}
