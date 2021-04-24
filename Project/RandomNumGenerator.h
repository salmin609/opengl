#include <ctime>
#include <random>
#include "Affine.h"

class RandomNumber
{
public:
	RandomNumber();
	float RandomFloat(float min, float max);
	int RandomInt(int max);
	Vector3 RandomVector3(float min, float max);
	static RandomNumber* instance;

private:
	std::random_device rd;
};