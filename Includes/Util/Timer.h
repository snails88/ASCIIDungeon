#pragma once
#include <Core/Core.h>

namespace Craft 
{
	class CRAFT_API Timer
	{
	public:
		Timer(float targetTime = 1.f);

		void Tick(float deltaTime);
		void Reset();
		void SetTargetTime(float targetTime);
		inline bool IsTimeOut() const { return elapsedTime >= targetTime; }
		inline float GetTargetTime() const { return targetTime; }
	private:
		float elapsedTime = 0.f;	// 경과 시간 계산용
		float targetTime = 0.f;		// 타이머 목표 시간
	};
}
