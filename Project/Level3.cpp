#include "Level3.h"

#include "Graphic.h"
#include "Particle.h"

Level3::Level3()
{
	particle = new Particle();
	object = new Object(particle, Point{ 0.f, 0.f, 0.f }, nullptr);
}

void Level3::Load()
{
	Graphic::objects.clear();
	Graphic::objects.push_back(object);
}

void Level3::Update(float dt)
{
	particle->MoveParticle(dt);
}

void Level3::UnLoad()
{
}
