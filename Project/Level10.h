#pragma once
#include <vector>

#include "Affine.h"
#include "FluidParticle.h"
#include "State.h"

class Model;
class LoadedObj;
class VAO;
class Buffer;
class Shader;
class TgaTexture;

class FluidCompute;

class Level10 : public State
{
public:
	Level10();
	~Level10();

	void Load() override;
	void Update(float dt) override;
	void UnLoad() override;
private:
	FluidCompute* fluid;
	Shader* render;
	Shader* compute;
	Shader* computeNeighbor;
	int pTotalNum;
	int pxNum;
	int pyNum;
	int pzNum;
	std::vector<Vector3> colors;
	std::vector<Vector3> radii;

	std::vector<Vector4> predictedPos;
	std::vector<Vector4> pos;
	std::vector<Vector4> vel;
	std::vector<Vector4> force;
	std::vector<int> ids;
	std::vector<float> density;
	std::vector<float> lambda;
	std::vector<Neighbors> neighbors;
	std::vector<int> neighborsCheckCount;
	
	Buffer* vertexBuffer;
	Buffer* colorBuffer;
	Buffer* radiiBuffer;

	Buffer* particlePos;
	Buffer* particleForce;
	Buffer* particleVel;
	Buffer* particleId;
	Buffer* particleDensity;
	Buffer* particleLambda;
	Buffer* particlePredictedPos;
	Buffer* particleNeighbors;
	Buffer* particleNeighborsCheckCount;
};
