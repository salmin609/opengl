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

	unsigned vao;
	unsigned renderFbo;
	
	unsigned texScene;
	unsigned texBrightPass;
	unsigned texDepth;

	unsigned filterFbo[2];
	unsigned texFilter[2];

	unsigned texLut;
	unsigned uboTransform;
	unsigned uboMaterial;
	
	SnubDodecMesh snubMesh;
	Object* object;
	VAO* objectVao;
	LoadedObj* sphere;
	enum
	{
		MAX_SCENE_WIDTH = 2048,
		MAX_SCENE_HEIGHT = 2048,
		SPHERE_COUNT = 100,
	};

	float bloomThreshMin = 0.8f;
	float bloomThreshMax = 1.2f;
};
