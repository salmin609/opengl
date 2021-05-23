#pragma once
#include <vector>
#include "Affine.h"
#include "State.h"
#include "FluidParticle.h"

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
	Shader* compute;
	Shader* render;

	Buffer* particleVec4Buffer;
	Buffer* particleValBuffer;
	Buffer* particleNeighborBuffer;
	Buffer* particlePosBuffer;
	Buffer* particleDensityBuffer;

	std::vector<ParticleVec42> particleVec4s;
	std::vector<ParticleVal> particleVals;
	std::vector<Neighbors> particleNeighbors;
	std::vector<Vector4> particlePoses;
	std::vector<float> particleDensities;
	
	
	const int pxNum = 20;
	const int pyNum = 20;
	const int pzNum = 20;
	const int pTotalNum = pxNum * pyNum * pzNum;
	
};
