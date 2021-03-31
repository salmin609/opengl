#include "Particle.h"

Particle::Particle()
{
	isInstancing = true;
	Particle::SetVertices();
}

void Particle::SetVertices()
{
	InitializeColoredParticle(shaderParticleColoredVertex, shaderParticleColoredFragment);
}
