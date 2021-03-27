#include "Level2.h"
#include "Graphic.h"
#include "LoadedObj.h"

Level2::Level2()
{
	emerald.ambient = Vector(0.0215f, 0.1745f, 0.0215f);
	emerald.diffuse = Vector(0.07568f, 0.61424f, 0.07568f);
	emerald.specular = Vector(0.633f, 0.727811f, 0.633f);
	emerald.shiness = 0.6f;

	objMesh = new LoadedObj("Dog", true, true);
	obj = new Object(objMesh, Point(0.f, 0.f, 0.f), &emerald);
	obj->Set_Scale(Vector(0.1f, 0.1f, 0.1f));
}

void Level2::Load()
{
	Graphic::objects.clear();
	Graphic::objects.push_back(obj);
	Graphic::ground = nullptr;
}

void Level2::Update()
{
	
}

void Level2::UnLoad()
{
}
