#pragma once
#include "CubeMesh.h"
#include "Object.h"
#include "SnubDodecMesh.h"
#include "State.h"

class Water;
class Object;

class Level4 : public State
{
public:
	Level4();
	void Load() override;
	void Update(float dt) override;
	void UnLoad() override;
private:
	Object* waterObj;
	Object* ground;
	Object* upFirstSnub;
	Object* upSecondSnub;
	Object* downFirstSnub;
	Object* downSecondSnub;
	Object* downCube;
	SnubDodecMesh snub_mesh;
	CubeMesh cube_mesh;
	Water* water;
	Material_ emeraldMat;
	Material_ obcidianMat;
};
