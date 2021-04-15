#include <ctime>
#include <random>
#include "Affine.h"

namespace RandomNumber
{
	inline static float RandomFloat(float min, float max)
	{
		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<> distr(static_cast<double>(min), static_cast<double>(max));
		return static_cast<float>(distr(eng));
	}
	inline static int RandomInt(int max)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(0, max);
		return dis(gen);
	}
	inline static Vector3 RandomVector3(float min, float max)
	{
		return Vector3{ RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max) };
	}
}
