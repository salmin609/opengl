#include "Level5.h"
#include "Graphic.h"


Level5::Level5()
{

	
}

Level5::~Level5()
{

}

void Level5::Load()
{
	Graphic::objects.clear();
	Graphic::water = nullptr;
	CameraManager::instance->SetCameraPos(Vector3{ 0.942832f, 1.50537f, -1.57533f }, Vector3{ 0.f, -0.1f, 2.f });
}

void Level5::Update(float dt)
{
	(dt);
	
}

void Level5::UnLoad()
{
}

void Level5::ProgramInit()
{
	
}
