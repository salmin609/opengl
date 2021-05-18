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

	std::vector<ParticleVec4> particleVec4Vals;
	std::vector<ParticleVal> particleVals;
	std::vector<BubbleVec4> bubbleVec4Vals;
	
	std::vector<int> bubbleTypes;
	std::vector<float> bubbleRadiuses;
	std::vector<Vector4> bubblePoses;
	std::vector<Vector4> bubbleVelocities;
	std::vector<float> bubbleLifetimes;

	std::vector<int> bubbleTypeCheck;
	std::vector<Vector4> bubbleGeneratedPos;
	std::vector<Vector4> bubbleGeneratedColor;
	std::vector<float> bubbleGeneratedRadius;

	Buffer* particleValVec4Buffer;
	Buffer* particleValBuffer;
	
	Buffer* vertexBuffer;
	Buffer* colorBuffer;
	Buffer* radiiBuffer;
	Buffer* colorBufferBubble;
	Buffer* radiiBufferBubble;
	

	Buffer* particlePos;
	Buffer* particleForce;
	Buffer* particleVel;
	Buffer* particleId;
	Buffer* particleDensity;
	Buffer* particleLambda;
	Buffer* particlePredictedPos;
	Buffer* particleNeighbors;
	Buffer* particleNeighborsCheckCount;

	Buffer* bubbleType;
	Buffer* bubbleRadius;
	Buffer* bubbleVec4;
	//Buffer* bubbleVel;
	Buffer* bubbleLifetime;

	float pointSize = 0.00008f;
	float bubbleSize = 0.00008f;
	TgaTexture* tgaTexture;
	int gfsCount;
	float fpsTimer;
};
