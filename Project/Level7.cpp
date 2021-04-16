#include "Level7.h"
#include "Light_Mesh.h"
#include "Graphic.h"
#include "Projection.h"
#include "Shader.h"
#include "Shader_Table.hpp"
#include "Texture.h"
#include "vs2017/SimpleMeshes.h"
#include "VAO.h"
#include <iostream>
#include "FrameBuffer.h"
Level7::Level7()
{
	renderingShader = new Shader(shaderFrameBufferVertex.c_str(), shaderFrameBufferFragment.c_str());
	renderingShader->Use();
	renderingShader->SendUniformInt("screenTextures", 0);

	computeShader = new Shader(shaderComputeRaytracing.c_str());
	computeShader->Use();

	texture = new Texture(GL_RGBA32F);
	texture->BindImageTexture(0, GL_WRITE_ONLY);

	frameBufferObj = new FrameBufferObject(texture, shaderFrameBufferVertex, shaderFrameBufferFragment);

	/*light_mesh = new Light_Mesh();
	bottomwallObj = new Object(&cube_mesh, Point{ 0.f, -3.f, 0.f }, &obsidian);
	topwallObj = new Object(&cube_mesh, Point{ 0.f, 3.f, 0.f }, &obsidian);
	leftwallObj = new Object(&cube_mesh, Point{ -3.f, 0.f, 0.f }, &obsidian);
	rightwallObj = new Object(&cube_mesh, Point{ 3.f, 0.f, 0.f }, &obsidian);
	backwallObj = new Object(&cube_mesh, Point{ 0.f, 0.f, -3.f }, &obsidian);
	frontwallObj = new Object(&cube_mesh, Point{ 0.f, 0.f, 3.f }, &obsidian);
	light = new Object(light_mesh, Point{ 0.f, 0.f, 0.f }, &pearl, Vector{ 1.f,1.f,1.f });
	light->Set_Scale(0.35f);
	const float longDist = 3.5f;
	const float shortDist = 0.5f;

	bottomwallObj->Set_Scale(Vector{ longDist,shortDist,longDist });
	topwallObj->Set_Scale(Vector{ longDist,shortDist,longDist });
	leftwallObj->Set_Scale(Vector{ shortDist,longDist,longDist });
	rightwallObj->Set_Scale(Vector{ shortDist,longDist,longDist });
	backwallObj->Set_Scale(Vector{ longDist,longDist,shortDist });
	frontwallObj->Set_Scale(Vector{ longDist,longDist,shortDist });*/
	
	/*Point lightPos = light->Get_Obj_Pos();
	bottomwallObj->Set_Light_Pos(lightPos);
	leftwallObj->Set_Light_Pos(lightPos);
	rightwallObj->Set_Light_Pos(lightPos);
	frontwallObj->Set_Light_Pos(lightPos);
	backwallObj->Set_Light_Pos(lightPos);
	topwallObj->Set_Light_Pos(lightPos);*/

	//delete[] simpleRect;
}

Level7::~Level7()
{
	/*delete circleObj;
	delete bottomwallObj;
	delete leftwallObj;
	delete rightwallObj;
	delete frontwallObj;
	delete backwallObj;
	delete topwallObj;*/
}

void Level7::Load()
{
	Graphic::objects.clear();
	Graphic::water = nullptr;
	CameraManager::instance->SetCameraPos(Vector3{ 0.942832f, 1.50537f, -1.57533f }, Vector3{ 0.f, -0.1f, 2.f });
}

void Level7::Update(float dt)
{
	(dt);
	glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	glViewport(0, 0, 1024, 768);
	glClear(GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	computeShader->Use();
	const Point camPos = CameraManager::instance->GetCamera()->Eye();
	Vector3 pos{ camPos.x, camPos.y, camPos.z };

	Matrix camMat = WorldToCamera(*CameraManager::instance->GetCamera());
	Matrix ndcMat = CameraToNDC(*CameraManager::instance->GetCamera());
	Matrix inverseViewProjectMatrix = inverse(ndcMat * camMat);

	Hcoord ray00val = inverseViewProjectMatrix * Hcoord(-1.f, -1.f, 0.f, 1.f);
	ray00val /= ray00val.w;
	ray00val = ray00val - camPos;

	Hcoord ray01val = inverseViewProjectMatrix * Hcoord(-1.f, 1.f, 0.f, 1.f);
	ray01val /= ray01val.w;
	ray01val = ray01val - camPos;

	Hcoord ray10val = inverseViewProjectMatrix * Hcoord(1.f, -1.f, 0.f, 1.f);
	ray10val /= ray10val.w;
	ray10val = ray10val - camPos;

	Hcoord ray11val = inverseViewProjectMatrix * Hcoord(1.f, 1.f, 0.f, 1.f);
	ray11val /= ray11val.w;
	ray11val = ray11val - camPos;

	Vector3 ray00 = Vector3{ ray00val.x, ray00val.y, ray00val.z };
	Vector3 ray01 = Vector3{ ray01val.x, ray01val.y, ray01val.z };
	Vector3 ray10 = Vector3{ ray10val.x, ray10val.y, ray10val.z };
	Vector3 ray11 = Vector3{ ray11val.x, ray11val.y, ray11val.z };

	computeShader->SendUniformVec3("eye", &pos);
	computeShader->SendUniformVec3("ray00", &ray00);
	computeShader->SendUniformVec3("ray01", &ray01);
	computeShader->SendUniformVec3("ray10", &ray10);
	computeShader->SendUniformVec3("ray11", &ray11);

	glDispatchCompute(64, 96, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	frameBufferObj->Bind();
	frameBufferObj->UnBind();
	frameBufferObj->Use();
}

void Level7::UnLoad()
{
}
