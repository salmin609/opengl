#ifndef TEST_CUH
#define TEST_CUH
#include "SandParticle.h"


void SetPosition(ParticleSand* pos);
void Init(int particleNum, int gridNum, ParticleSand* particle, ParticleGrid* grid, Land* land);
void SandUpdate(int particleNum, int gridNum, ParticleSand* particle, ParticleGrid* grid);
#endif