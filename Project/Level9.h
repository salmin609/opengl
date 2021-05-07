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
	Shader* render;
	Shader* compute;
	Buffer* positionBuffer;
	Buffer* velocityBuffer;
	Buffer* lifeBuffer;
	Buffer* attratorBuffer;

	std::vector<Vector3> positionDatas;
	std::vector<Vector4> velocityDatas;
	std::vector<Vector4> attratorDatas;
	std::vector<float> lifeDatas;
	
	int particleCountX;
	int particleCountY;
	int totalParticleNum;
	int attractorCount;
	int gfsCount;
	float fpsAll;
};
