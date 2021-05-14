#pragma once
#include <vector>

#include "Affine.h"
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
	int pTotalNum;
	std::vector<Vector3> colors;
	std::vector<Vector3> radii;

	Buffer* vertexBuffer;
	Buffer* colorBuffer;
	Buffer* radiiBuffer;
};
