#ifndef TEST_CUH
#define TEST_CUH
#include "SandParticle.h"


void SetPosition(ParticleSand* pos);
void Init(int particleNum, int gridNum, int spawnerNum, ParticleSand* particle, ParticleGrid* grid, Land* land, SpawnerPos* spawners);
void SetLands(std::vector<int>& landStartRandomIndices, ParticleSand* particle, ParticleGrid* grid, Land* land);
void SandUpdate(int particleNum, int gridNum, ParticleSand* particle, ParticleGrid* grid);
void AddSandsInSpawnerPos(ParticleSand* particle, ParticleGrid* grid, SpawnerPos* spawners, int lastIndex);
void AddLandsInSpawnerPos(Land* lands, ParticleGrid* grid, SpawnerPos* spawners, int lastIndex);
void MoveSpawner(ParticleGrid* grid, SpawnerPos* spawners, int dir, int spawnerCount);
void LoadLands(int* landGridIndices, ParticleGrid* grid, Land* land, int landsNum);
void DeleteLands(Land* lands, ParticleGrid* grids, SpawnerPos* spawners);
#endif