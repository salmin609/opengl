#pragma once
#include "CubeMesh.h"
#include "Object.h"
#include "SnubDodecMesh.h"
#include "State.h"
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

private:
	Texture* computeInput;
	Texture* computeOutput;
	VAO* vao;
	Shader* shaderCheck;
	Shader* shaderTexture;
	int imgWidth, imgHeight;
	unsigned checkTexture;

	Object* circleObj;
	Light_Mesh* light_mesh;
	Object* bottomwallObj;
	Object* leftwallObj;
	Object* rightwallObj;
	Object* frontwallObj;
	Object* backwallObj;
	Object* topwallObj;

	Object* light;
	
	SnubDodecMesh snub_mesh;
	CubeMesh cube_mesh;
	Material_ obsidian;
	Material_ pearl;
};
