#pragma once

#include "Object.h"
#include "State.h"
#include "SnubDodecMesh.h"
class FrameBufferObject;
class Light_Mesh;
class Object;
class Shader;
class Texture;
class VAO;
class LoadedObj;
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

	//unsigned vao;
	unsigned renderFbo;
	
	unsigned texScene;
	unsigned texBrightPass;
	unsigned texDepth;

	unsigned filterFbo[2];
	unsigned texFilter[2];

	//unsigned texLut;
	unsigned uboTransform;
	unsigned uboMaterial;
	
	SnubDodecMesh snubMesh;
	Object* object;
	VAO* objectVao;
	LoadedObj* sphere;
	int screenWidth;
	int screenHeight;
	enum
	{
		SPHERE_COUNT = 1000,
	};
	struct transforms_t
	{
		Matrix mat_proj;
		Matrix mat_view;
		Matrix mat_model[SPHERE_COUNT];
	};
	std::vector<Vector3> randomPos;

	float bloomThreshMin = 1.0f;
	float bloomThreshMax = 1.2f;
};
