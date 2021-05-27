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
	void SetCamVariables();
private:
	Shader* compute;
	Shader* render;
	Shader* boxRender;

	Buffer* particleVec4Buffer;
	Buffer* particleValBuffer;
	Buffer* particleNeighborBuffer;
	Buffer* particlePosBuffer;
	Buffer* particleDensityBuffer;
	Buffer* particleIndicesBuffer;
	
	Buffer* boxPositionBuffer;
	Buffer* boxNormalBuffer;

	std::vector<ParticleVec42> particleVec4s;
	std::vector<ParticleVal> particleVals;
	std::vector<Neighbors> particleNeighbors;
	std::vector<Vector4> particlePoses;
	std::vector<float> particleDensities;
	std::vector<CheckIndices> particleIndices;
	
	const float pDist = 0.08f;
	const int pxNum = 20;
	const int pyNum = 20;
	const int pzNum = 20;
	const int pTotalNum = pxNum * pyNum * pzNum;

	Matrix camMat;
	Matrix ndcMat;
	Point camEye;
	Matrix mvp;

	const Vector3 halfDimensions = Vector3(0.8f, 1.0f, 1.0f);
	float halfx = halfDimensions.x;
	float halfy = halfDimensions.y;
	float halfz = halfDimensions.z;
	
};
