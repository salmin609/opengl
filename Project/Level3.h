#pragma once
#include "State.h"
class Particle;
class Object;
class Level3 : public State
{
public:
	Level3();
	void Load() override;
	void Update(float dt) override;
	void UnLoad() override;

private:
	Particle* particle;
	Object* object;
};