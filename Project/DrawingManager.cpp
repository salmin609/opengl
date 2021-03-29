#include "DrawingManager.h"
#include "Graphic.h"
#include "Projection.h"
#include <iostream>
#include "FrameBuffer.h"

DrawingManager::DrawingManager()
{
	outlineShader = new Shader(shader_object_vertex.c_str(),
		shaderSingleColorFragment.c_str());

	frameBufferObj = new FrameBufferObject();
}

void DrawingManager::Drawing()
{
	ndcMat = CameraToNDC(*CameraManager::instance->GetCamera());
	camMat = WorldToCamera(*CameraManager::instance->GetCamera());
	objectsSize = Graphic::objects.size();
	object = Graphic::objects;

	frameBufferObj->Bind();
	ClearBuffer();
	
	DrawingGround();
	DrawingShadow();
	OutlinePrepare();
	DrawingObjs();
	DrawingOutline();

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
		Graphic::ground->Draw(ndcMat, camMat);
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
		obj->Draw(ndcMat, camMat);
	}
}

void DrawingManager::DrawingOutline()
{
	Object* selectedObj = Graphic::instance->SelectedObject();

	if (selectedObj != nullptr)
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		outlineShader->Use();
		outlineShader->SendUniformMat("to_ndc", &ndcMat);
		outlineShader->SendUniformMat("cam", &camMat);

		selectedObj->SetVAO();

		Matrix model2World = selectedObj->GetModelToWorldOutlineScaling(0.1f);
		Mesh* mesh = selectedObj->Get_Mesh();
		outlineShader->SendUniformMat("model", &model2World);

		
		if(selectedObj->IsElemented())
		{
			glDrawElements(GL_TRIANGLES, mesh->FaceCount() * 3, GL_UNSIGNED_INT, nullptr);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, mesh->FaceCount() * 3);
		}
		
		glBindVertexArray(0);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}
}

void DrawingManager::OutlinePrepare()
{
	Object* selectedObj = Graphic::instance->SelectedObject();

	if(selectedObj != nullptr)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}
}
