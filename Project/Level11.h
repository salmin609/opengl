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
	Buffer* sandPosBuffer;
	Buffer* gridPosBuffer;
	Buffer* landPosBuffer;
	
	ParticleSand* sandParticles;
	ParticleSand* dSandParticles;
	ParticleGrid* sandGrids;
	ParticleGrid* dSandGrids;
	Land* lands;
	Land* dLands;

	float timer = 1.f;
	
	const int NUMPARTICLES = 256 * 4;
	const int particleMemSize = NUMPARTICLES * sizeof(ParticleSand);
	
	const int NUMGRIDS = 256 * 20;
	const int gridMemSize = NUMGRIDS * sizeof(ParticleGrid);

	const int NUMLANDS = 256 * 4;
	const int landMemSize = NUMLANDS * sizeof(Land);
};
