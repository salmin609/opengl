#pragma once
#include "Object.h"
#include "State.h"
class FrameBufferObject;
class Light_Mesh;
class Object;
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
	void ProgramInit();
private:
	Shader* render;
	Shader* filter;
	Shader* resolve;
};
