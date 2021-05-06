#pragma once
#include "State.h"

class Model;
class LoadedObj;
class VAO;
class Buffer;
class Shader;

class Level9 : public State
{
public:
	Level9();
	~Level9();

	void Load() override;
	void Update(float dt) override;
	void UnLoad() override;
private:
	struct ParticlePos
	{
		float x;
		float y;
		float z;
		float w; //speed
	};
	struct ParticleVelocity
	{
		float vx;
		float vy;
		float vz;
		float fTimeToLive; //Remaining time. 
	};

	Shader* renderShader;
	Shader* computeShader;
	Buffer* positionBuffer[2];
	Buffer* velocityBuffer[2];
	int particleCount;
	unsigned numworkGroups[3];
	VAO* drawVao[2];
	ParticlePos* particlePositions;
	ParticleVelocity* particleVelocity;
	int index = 0;
	LoadedObj* loadedObj;
	Model* model;
	unsigned positionBufferSize;
	unsigned velocityBufferSize;
};
