#pragma once
#include <vector>
#include "Affine.h"
#include "State.h"
#include "SandParticle.h"

class Model;
class LoadedObj;
class VAO;
class Buffer;
class Shader;
class TgaTexture;
class FluidCompute;

class Level11 : public State
{
public:
	Level11();
	~Level11();

	void Load() override;
	void Update(float dt) override;
	void UnLoad() override;
private:
	void CopyFromDeviceToHost();
	void CopyFromHostToDevice();
	Shader* render;
	Shader* renderOnePoint;
	Buffer* sandPosBuffer;
	Buffer* gridPosBuffer;
	Buffer* landPosBuffer;
	Buffer* spawnerPosBuffer;
	
	ParticleSand* sandParticles;
	ParticleSand* dSandParticles;
	ParticleGrid* sandGrids;
	ParticleGrid* dSandGrids;
	Land* lands;
	Land* dLands;
	int* dLoadedLands;
	int* loadedLands;
	SpawnerPos* spawnerPos;
	SpawnerPos* dSpawnerPos;

	float timer = 1.f;
	
	int NUMPARTICLES = 256 * 30;
	int particleMemSize = NUMPARTICLES * sizeof(ParticleSand);
	
	int NUMGRIDS = 256 * 2000;
	int gridMemSize = NUMGRIDS * sizeof(ParticleGrid);

	int NUMLANDS = 256 * 4;
	int landMemSize = NUMLANDS * sizeof(Land);

	int NUMSPAWNERS = 256 * 1;
	int spawnerMemSize = NUMSPAWNERS * sizeof(SpawnerPos);
};
