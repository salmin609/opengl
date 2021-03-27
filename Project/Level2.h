#pragma once
#include "Object.h"
#include "State.h"

class LoadedObj;

class Level2 : public State
{
public:
	Level2();
	void Load() override;
	void Update() override;
	void UnLoad() override;
private:
	LoadedObj* objMesh;
	Object* obj;
	Material_ emerald;
};
