#include "Particle.h"
#include "RandomNumGenerator.h"

Particle::Particle()
{
	isInstancing = true;
	Particle::SetVertices();
}

void Particle::SetVertices()
{
	timer = 2.f;
	instancingNum = 0;
	particleNum = 1000;

	for (size_t i = 0; i < particleNum; ++i)
	{
		const float randomx = RandomNumber::RandomFloat(0.f, 1.f);
		const float randomy = RandomNumber::RandomFloat(0.f, 1.f);
		const float randomz = RandomNumber::RandomFloat(0.f, 1.f);

		const float randomDirX = RandomNumber::RandomFloat(0.f, 10.f);
		const float randomDirY = RandomNumber::RandomFloat(0.f, 10.f);
		const float randomDirZ = RandomNumber::RandomFloat(0.f, 10.f);

		const float randomRotX = RandomNumber::RandomFloat(0.f, 10.f);
		const float randomRotY = RandomNumber::RandomFloat(0.f, 10.f);
		const float randomRotZ = RandomNumber::RandomFloat(0.f, 10.f);

		ParticleInstance data{ 2.f, Vector3{
			randomx,
			randomy,
			randomz
		},
		Vector3{
			randomDirX,
			randomDirY,
			randomDirZ
		},
		Vector3{
			randomRotX,
			randomRotY,
			randomRotZ
		} };
		
		particles.push_back(data);
	}
	InitializeColoredParticle(shaderParticleColoredVertex, shaderParticleColoredFragment);
}
