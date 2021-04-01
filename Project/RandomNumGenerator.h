#include <random>

namespace RandomNumber
{
	inline static float RandomFloat(float min, float max)
	{
		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_real_distribution<> distr(static_cast<double>(min), static_cast<double>(max));
		return static_cast<float>(distr(eng));
	}
}
