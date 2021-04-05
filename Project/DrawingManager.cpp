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
	frameBufferObj = new FrameBufferObject(0);
	waterReflectframeBufferObj = new FrameBufferObject(0, 200, 200);
	waterReflactframeBufferObj = new FrameBufferObject(0, 200, 200);

	//Graphic::water->Get_Mesh()->PushTextureId(waterReflectframeBufferObj->GetTextureColorBufferId());
	//Graphic::water->Get_Mesh()->PushTextureId(waterReflactframeBufferObj->GetTextureColorBufferId());
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
	
	//Reflection Texture


	if(Graphic::water != nullptr)
	{
		glEnable(GL_CLIP_DISTANCE0);

		/*Camera* cam = CameraManager::instance->GetCamera();
		Camera invertedCamera = *cam;
		const float distance = 2 * (cam->Eye().y - 1.f);
		invertedCamera.ChangeCameraYPosition(distance);
		const Matrix invertedCamMat = WorldToCamera(invertedCamera);
		const Matrix invertedTransposedCamMat = transpose(invertedCamMat);
		glBindBuffer(GL_UNIFORM_BUFFER, Graphic::instance->GetUboMatricesId());
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Affine), sizeof(Affine), &invertedTransposedCamMat);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);*/
		
		waterReflectframeBufferObj->Bind();
		waterReflectframeBufferObj->SetViewPort();
		ClearBuffer();
		WaterInitialize(Hcoord{0, 1, 0, -1.f});
		DrawingObjs();
		waterReflectframeBufferObj->UnBind();
		waterReflectframeBufferObj->ResetViewPort();
		//glBindBuffer(GL_UNIFORM_BUFFER, Graphic::instance->GetUboMatricesId());
		//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Affine), sizeof(Affine), &transposed);
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//Reflaction Texture
		waterReflactframeBufferObj->Bind();
		waterReflactframeBufferObj->SetViewPort();
		ClearBuffer();
		WaterInitialize(Hcoord{ 0, -1, 0, 1.f });
		DrawingObjs();
		waterReflactframeBufferObj->UnBind();
		waterReflactframeBufferObj->ResetViewPort();
		glDisable(GL_CLIP_DISTANCE0);
	}

	//WaterInitialize(Hcoord{ 0, -1, 0, 10000 });
	frameBufferObj->Bind();
	ClearBuffer();
	DrawingGround();
	DrawingShadow();
	DrawingWater();
	outLine->OutlinePrepare();
	DrawingObjs();
	outLine->Draw();
	frameBufferObj->UseFrameBuffer(waterReflectframeBufferObj, 0, 600, 200, 400);
	frameBufferObj->UseFrameBuffer(waterReflactframeBufferObj, 400, 400, 600, 600);
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

void DrawingManager::DrawingWater()
{
	Object* waterObj = Graphic::water;

	if(waterObj != nullptr)
	{
		glUseProgram(waterObj->Get_Mesh()->Get_Shader_Id());
		
		waterObj->Get_Mesh()->GetShader()->SendUniformInt("reflect", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, waterReflectframeBufferObj->GetTextureColorBufferId());
		waterObj->Get_Mesh()->GetShader()->SendUniformInt("refract", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, waterReflactframeBufferObj->GetTextureColorBufferId());
		
		waterObj->Draw();
	}
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

void DrawingManager::WaterInitialize(Hcoord planeVec)
{
	if(Graphic::water != nullptr)
	{
		Object* waterObj = Graphic::water;
		const Point waterPos = waterObj->Get_Obj_Pos();
		for(size_t i = 0 ; i < objectsSize; ++i)
		{
			Object* obj = object[i];
			obj->SetPlane(planeVec);
		}
	}
}
