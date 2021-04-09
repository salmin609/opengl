#pragma once
#include "State.h"
class Shader;
class Texture;
class VAO;
class Level5 : public State
{
public:
	Level5();
	~Level5();
	void Load() override;
	void Update(float dt) override;
	void UnLoad() override;
private:
	Texture* computeInput;
	Texture* computeOutput;
	VAO* vao;
	Shader* shaderCheck;
	Shader* shaderTexture;
	int imgWidth, imgHeight;
};
