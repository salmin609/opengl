#include "DrawingManager.h"
#include "Graphic.h"
#include "Projection.h"
#include <iostream>
#include "FrameBuffer.h"
#include "Skybox.h"
#include "OutLine.h"

DrawingManager::DrawingManager()
{
	outLine = new OutLine();
	frameBufferObj = new FrameBufferObject();
	//skyBox = new SkyBox();
}

void DrawingManager::Drawing()
{
	ndcMat = CameraToNDC(*CameraManager::instance->GetCamera());
	camMat = WorldToCamera(*CameraManager::instance->GetCamera());
	objectsSize = Graphic::objects.size();
	object = Graphic::objects;

	Matrix transposed = transpose(camMat);
	glBindBuffer(GL_UNIFORM_BUFFER, Graphic::instance->GetUboMatricesId());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Affine), sizeof(Affine), &transposed);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	frameBufferObj->Bind();
	
	ClearBuffer();
	DrawingGround();
	DrawingShadow();
	outLine->OutlinePrepare();
	DrawingObjs();
	outLine->Draw();
	//skyBox->Draw(ndcMat);
	frameBufferObj->UnBind();
	frameBufferObj->Use();
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
	if(Graphic::ground != nullptr)
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
	for(size_t i = 0 ; i < objectsSize; ++i)
	{
		Object* obj = object[i];
		Object_ExceptShadow* shadow = obj->Get_Shadow();
		Matrix objModelMat = obj->Get_Model_To_World();

		if(shadow != nullptr)
		{
			shadow->Set_Light_Pos(lightPos);
			shadow->Draw(ndcMat, camMat, objModelMat, shadowMat);
		}
	}
	glEnable(GL_DEPTH_TEST);
}

void DrawingManager::DrawingObjs()
{
	for(size_t i = 0; i < objectsSize; ++i)
	{
		Object* obj = object[i];
		obj->Set_Camera_Pos(CameraManager::instance->CameraPos());
		obj->Set_Light_Pos(Graphic::light->Get_Obj_Pos());
		obj->Draw();
	}
}