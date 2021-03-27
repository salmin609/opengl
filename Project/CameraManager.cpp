#include "CameraManager.h"
#include <iostream>
#include "Graphic.h"

CameraManager* CameraManager::instance = nullptr;

CameraManager::CameraManager()
{
	if(instance != nullptr)
	{
		std::cout << "There are two cameraManager delete one" << std::endl;
		delete instance;
	}
	instance = this;
	Init();
}

void CameraManager::Init()
{
	cam_alpha = 0.f;
	cam_beta = pi;
	r = 1.f;
	SetVV();
}

void CameraManager::Move(SDL_Keycode keycode)
{
	float speed = 0.1f;

	if (move_faster)
	{
		speed = 0.5f;
	}

	Vector cameraRight = cam.Right() * speed;
	switch (keycode)
	{
	case SDLK_w:
		defaultCamPos.x += (v_v.x * speed);
		defaultCamPos.y += (v_v.y * speed);
		defaultCamPos.z += (v_v.z * speed);
		break;
	case SDLK_a:
		defaultCamPos.x -= cameraRight.x;
		defaultCamPos.y -= cameraRight.y;
		defaultCamPos.z -= cameraRight.z;
		break;
	case SDLK_s:
		defaultCamPos.x -= (v_v.x * speed);
		defaultCamPos.y -= (v_v.y * speed);
		defaultCamPos.z -= (v_v.z * speed);
		break;
	case SDLK_d:
		defaultCamPos.x += cameraRight.x;
		defaultCamPos.y += cameraRight.y;
		defaultCamPos.z += cameraRight.z;
		break;
	}

	cam = Camera(defaultCamPos, lookat, ey, 0.5f * pi, 1.f, aspect, 100.f);
}

void CameraManager::CameraMovement(SDL_Event event)
{
	float speed = 0.1f;

	if (move_faster)
	{
		speed = 0.5f;
	}

	const bool is_mouse_wheel_up = event.wheel.y > 0;

	if (traverse_mode)
	{
		if (is_mouse_wheel_up)
		{
			defaultCamPos.x += (v_v.x * speed);
			defaultCamPos.y += (v_v.y * speed);
			defaultCamPos.z += (v_v.z * speed);
		}
		cam = Camera(defaultCamPos, lookat, ey, 0.5f * pi, 1.f, 0.5f, 100.f);
	}
	else
	{
		if (is_mouse_wheel_up)
		{
			if (Graphic::instance->SelectedObject() != nullptr)
			{
				Point& obj_pos = Graphic::instance->SelectedObject()->Get_Obj_Pos();
				obj_pos.z += (0.1f);
			}
		}
	}

	if (traverse_mode == true)
	{
		if (!is_mouse_wheel_up)
		{
			defaultCamPos.x -= (v_v.x * speed);
			defaultCamPos.y -= (v_v.y * speed);
			defaultCamPos.z -= (v_v.z * speed);
		}
		cam = Camera(defaultCamPos, lookat, Vector(0, 1, 0), 0.5f * pi, 1.f, aspect, 100.f);
	}
	else
	{
		if (!is_mouse_wheel_up)
		{
			if (Graphic::instance->SelectedObject() != nullptr)
			{
				Point& obj_pos = Graphic::instance->SelectedObject()->Get_Obj_Pos();
				obj_pos.z -= (0.1f);
			}
		}
	}
}

void CameraManager::SetMoveFaster(bool toggle)
{
	move_faster = toggle;
}

void CameraManager::SetTraverseMode(bool toggle)
{
	traverse_mode = toggle;
}

void CameraManager::SetVV()
{
	v_v.x = r * cos(cam_alpha) * sin(cam_beta);
	v_v.y = r * sin(cam_alpha);
	v_v.z = r * cos(cam_alpha) * cos(cam_beta);
	lookat = v_v;
	cam = Camera(defaultCamPos, lookat, ey, 0.5f * pi, 1.f, 0.5f, 100.f);
}

Camera* CameraManager::GetCamera()
{
	return &cam;
}

Point CameraManager::CameraPos()
{
	return defaultCamPos;
}

void CameraManager::UpdateAB(float newAlpha, float newBeta)
{
	cam_alpha += newAlpha;
	cam_beta += newBeta;

	if (cam_alpha < -(pi / 2))
	{
		cam_alpha = -(pi / (2.0001f));
	}
	if (cam_alpha > (pi / 2))
	{
		cam_alpha = (pi / 2);
	}
}


