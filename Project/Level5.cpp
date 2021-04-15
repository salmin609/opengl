#include "Level5.h"
#include "Light_Mesh.h"
#include "Graphic.h"
#include "Projection.h"
#include "Shader.h"
#include "Shader_Table.hpp"
#include "Texture.h"
#include "vs2017/SimpleMeshes.h"
#include "VAO.h"
#include <iostream>
Level5::Level5()
{
	obsidian.ambient = Vector(0.05375f, 0.05f, 0.06625f);
	obsidian.diffuse = Vector(0.18275f, 0.17f, 0.22525f);
	obsidian.specular = Vector(0.332741f, 0.328634f, 0.346435f);
	obsidian.shiness = 0.3f;

	pearl.ambient = Vector(0.25f, 0.20725f, 0.20725f);
	pearl.diffuse = Vector(1.f, 0.829f, 0.829f);
	pearl.specular = Vector(0.296648f, 0.296648f, 0.296648f);
	pearl.shiness = 0.088f;

	light_mesh = new Light_Mesh();
	bottomwallObj = new Object(&cube_mesh, Point{ 0.f, -3.f, 0.f }, &obsidian);
	topwallObj = new Object(&cube_mesh, Point{ 0.f, 3.f, 0.f }, &obsidian);
	leftwallObj = new Object(&cube_mesh, Point{ -3.f, 0.f, 0.f }, &obsidian);
	rightwallObj = new Object(&cube_mesh, Point{ 3.f, 0.f, 0.f }, &obsidian);
	backwallObj = new Object(&cube_mesh, Point{ 0.f, 0.f, -3.f }, &obsidian);
	frontwallObj = new Object(&cube_mesh, Point{ 0.f, 0.f, 3.f }, &obsidian);
	light = new Object(light_mesh, Point{ 0.f, 0.f, 0.f }, &pearl, Vector{1.f,1.f,1.f});
	light->Set_Scale(0.35f);
	const float longDist = 3.5f;
	const float shortDist = 0.5f;
	
	bottomwallObj->Set_Scale(Vector{ longDist,shortDist,longDist });
	topwallObj->Set_Scale(Vector{ longDist,shortDist,longDist });
	leftwallObj->Set_Scale(Vector{ shortDist,longDist,longDist });
	rightwallObj->Set_Scale(Vector{ shortDist,longDist,longDist });
	backwallObj->Set_Scale(Vector{ longDist,longDist,shortDist });
	frontwallObj->Set_Scale(Vector{ longDist,longDist,shortDist });

	computeInput = new Texture("container.png", false, GL_RGBA32F);
	computeInput->BindImageTexture(0, GL_READ_ONLY);

	computeOutput = new Texture(GL_RGBA32F);
	computeOutput->BindImageTexture(1, GL_WRITE_ONLY);

	shaderCheck = new Shader(shaderCompute.c_str());
	shaderCheck->Use();

	shaderTexture = new Shader(shaderFrameBufferVertex.c_str(), shaderFrameBufferFragment.c_str());
	shaderTexture->Use();
	shaderTexture->SendUniformInt("screenTextures", 0);

	int count = 0;
	const std::vector<int> sizePerIndex{ 2, 2 };

	float* simpleRect = SimpleMesh::SimpleRectangleWithTextureCoord(count);
	vao = new VAO(shaderTexture);
	vao->Init(simpleRect, count, 2, sizePerIndex);

	Point lightPos = light->Get_Obj_Pos();
	bottomwallObj->Set_Light_Pos(lightPos);
	leftwallObj->Set_Light_Pos(lightPos);
	rightwallObj->Set_Light_Pos(lightPos);
	frontwallObj->Set_Light_Pos(lightPos);
	backwallObj->Set_Light_Pos(lightPos);
	topwallObj->Set_Light_Pos(lightPos);
	
	delete[] simpleRect;
}

Level5::~Level5()
{
	delete circleObj;
	delete bottomwallObj;
	delete leftwallObj;
	delete rightwallObj;
	delete frontwallObj;
	delete backwallObj;
	delete topwallObj;
}

void Level5::Load()
{
	Graphic::objects.clear();
	Graphic::water = nullptr;
	CameraManager::instance->SetCameraPos(Vector3{ 0.72f, 0.04f, -2.07f}, Vector3{ 0.f, -0.1f, 1.f });
}

void Level5::Update(float dt)
{
	(dt);
	/*shaderCheck->Use();
	glDispatchCompute(1024, 768, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);*/

	glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	glViewport(0, 0, 1024, 768);
	glClear(GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	Camera* cam = CameraManager::instance->GetCamera();
	Point camPos = cam->Eye();
	
	std::cout << camPos.x << " " << camPos.y << " " << camPos.z << std::endl;
	
	Matrix camMat = WorldToCamera(*CameraManager::instance->GetCamera());
	Matrix transposed = transpose(camMat);
	glBindBuffer(GL_UNIFORM_BUFFER, Graphic::instance->GetUboMatricesId());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Affine), sizeof(Affine), &transposed);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	light->Draw();
	
	bottomwallObj->Draw();
	topwallObj->Draw();
	leftwallObj->Draw();
	rightwallObj->Draw();
	frontwallObj->Draw();
	backwallObj->Draw();
	//vao->Bind();
	//computeOutput->Bind(0);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Level5::UnLoad()
{
}
