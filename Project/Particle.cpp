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
	instancingNum = 100;
	particleNum = 100;

	for (size_t i = 0; i < particleNum; ++i)
	{
		const float randomx = RandomNumber::RandomFloat(0.f, 1.f);
		const float randomy = RandomNumber::RandomFloat(0.f, 1.f);
		const float randomz = RandomNumber::RandomFloat(0.f, 1.f);

		ParticleInstance data{ 2.f, Vector3{
			static_cast<float>(randomx),
			static_cast<float>(randomy),
			static_cast<float>(randomz)
		} };
		
		particles.push_back(data);
	}
	
	for (size_t i = 0; i < particleNum; ++i)
	{
		const float randomx = RandomNumber::RandomFloat(0.f, 10.f);
		const float randomy = RandomNumber::RandomFloat(0.f, 10.f);
		const float randomz = RandomNumber::RandomFloat(0.f, 10.f);

		particles[i].dir = Vector3{
			static_cast<float>(randomx),
			static_cast<float>(randomy),
			static_cast<float>(randomz)
		};
		/*particleTranslation.push_back(Vector{
			static_cast<float>(randomx),
			static_cast<float>(randomy),
			static_cast<float>(randomz)
			});*/
	}

	for(size_t i = 0 ; i < particleNum; ++i)
	{
		const float randomx = RandomNumber::RandomFloat(0.f, 10.f);
		const float randomy = RandomNumber::RandomFloat(0.f, 10.f);
		const float randomz = RandomNumber::RandomFloat(0.f, 10.f);

		particles[i].rotate = Vector3{
			static_cast<float>(randomx),
			static_cast<float>(randomy),
			static_cast<float>(randomz)
		};
		/*particleRotate.push_back(Vector{
			static_cast<float>(randomx),
			static_cast<float>(randomy),
			static_cast<float>(randomz)
		});*/
	}
	
	InitializeColoredParticle(shaderParticleColoredVertex, shaderParticleColoredFragment);
}
