#pragma once
#include "Object.h"
#include "State.h"
class FrameBufferObject;
class Light_Mesh;
class Object;
class Shader;
class Texture;
class VAO;
class Level7 : public State
{
public:
	Level7();
	~Level7();
	void Load() override;
	void Update(float dt) override;
	void UnLoad() override;

private:
	Shader* renderingShader;
	Shader* computeShader;
	Texture* texture;
	FrameBufferObject* frameBufferObj;
};
