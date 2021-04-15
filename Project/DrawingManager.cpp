#include "DrawingManager.h"
#include "Graphic.h"
#include "Projection.h"
#include <iostream>
#include "FrameBuffer.h"
#include "Skybox.h"
#include "OutLine.h"
#include "WaterFrameBuffer.h"
#include "vs2017/FrameBufferTexturing.h"
#include "WaterRenderer.h"

DrawingManager::DrawingManager()
{
	outLine = new OutLine();
	frameBufferObj = new FrameBufferObject(0);
	//skyBox = new SkyBox();
	waterRenderer = new WaterRenderer();
}

void DrawingManager::Drawing(float dt)
{
	object = Graphic::objects;
	if (!object.empty())
	{
		objectsSize = Graphic::objects.size();

		ndcMat = CameraToNDC(*CameraManager::instance->GetCamera());
		camMat = WorldToCamera(*CameraManager::instance->GetCamera());
		Matrix transposed = transpose(camMat);
		glBindBuffer(GL_UNIFORM_BUFFER, Graphic::instance->GetUboMatricesId());
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Affine), sizeof(Affine), &transposed);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		const bool isWaterExist = Graphic::water != nullptr;

		if (isWaterExist)
		{
			glEnable(GL_CLIP_DISTANCE0);
			waterRenderer->GetReflectTexture()->Bind();
			WaterInitialize(Hcoord{ 0, 1, 0, -1.f + 0.1f });
			DrawingObjs();
			waterRenderer->GetReflectTexture()->UnBind();

			waterRenderer->GetRefractTexture()->Bind();
			WaterInitialize(Hcoord{ 0, -1, 0, 1.f });
			DrawingObjs();
			waterRenderer->GetRefractTexture()->UnBind();

			glDisable(GL_CLIP_DISTANCE0);
		}

		if (!isWaterExist)
		{
			frameBufferObj->Bind();
		}
		glViewport(0, 0, 1024, 768);
		ClearBuffer();
		DrawingGround();
		DrawingShadow();
		waterRenderer->Render(dt, ndcMat, camMat);
		//skyBox->Draw(ndcMat);
		outLine->OutlinePrepare();
		DrawingObjs();
		outLine->Draw();

		if (!isWaterExist)
		{
			frameBufferObj->UnBind();
			frameBufferObj->Use();
		}
	}
}

void DrawingManager::ClearBuffer()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void DrawingManager::DrawingGround()
{
	if (Graphic::ground != nullptr)
	{
		Point& lightPos = Graphic::light->Get_Obj_Pos();

		Graphic::ground->Set_Light_Pos(lightPos);
		Graphic::ground->Draw();
	}
}
void DrawingManager::DrawingShadow()
{
	Point& lightPos = Graphic::light->Get_Obj_Pos();

	glDisable(GL_DEPTH_TEST);
	Matrix shadowMat = Shadow(lightPos);
	for (size_t i = 0; i < objectsSize; ++i)
	{
		Object* obj = object[i];
		Object_ExceptShadow* shadow = obj->Get_Shadow();
		Matrix objModelMat = obj->Get_Model_To_World();

		if (shadow != nullptr)
		{
			shadow->Set_Light_Pos(lightPos);
			shadow->Draw(ndcMat, camMat, objModelMat, shadowMat);
		}
	}
	glEnable(GL_DEPTH_TEST);
}

void DrawingManager::DrawingObjs()
{
	for (size_t i = 0; i < objectsSize; ++i)
	{
		Object* obj = object[i];
		obj->Set_Camera_Pos(CameraManager::instance->CameraPos());
		obj->Set_Light_Pos(Graphic::light->Get_Obj_Pos());
		obj->Draw();
	}
}

void DrawingManager::DrawLight()
{
	Object* lightObj = Graphic::light;

	if (lightObj != nullptr)
	{
		lightObj->Draw();
	}
}

void DrawingManager::WaterInitialize(Hcoord planeVec)
{
	if (Graphic::water != nullptr)
	{
		Object* waterObj = Graphic::water;
		const Point waterPos = waterObj->Get_Obj_Pos();
		for (size_t i = 0; i < objectsSize; ++i)
		{
			Object* obj = object[i];
			obj->SetPlane(planeVec);
		}
	}
}
