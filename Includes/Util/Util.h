#pragma once

#include <random>

namespace Util
{
	inline std::mt19937& GetRandomEngine()		// 랜덤 엔진
	{
		static std::mt19937 engine;
		return engine;
	}

	inline void SetRandomSeed()
	{
		std::random_device randomDevice;
		GetRandomEngine().seed(randomDevice());
	}

	inline int RandomRange(int min, int max)
	{
		std::uniform_int_distribution<int> distribution(min, max);

		return distribution(GetRandomEngine());
	}

	inline float RandomRange(float min, float max)
	{
		std::uniform_real_distribution<float> distribution(min, max);

		return distribution(GetRandomEngine());
	}
}