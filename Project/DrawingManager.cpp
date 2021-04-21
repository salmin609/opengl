#include "vmath.h"

#include "DrawingManager.h"
#include "Graphic.h"
#include "Projection.h"
#include <iostream>

#include "Client.h"
#include "FrameBuffer.h"
#include "Skybox.h"
#include "OutLine.h"
#include "WaterFrameBuffer.h"
#include "vs2017/FrameBufferTexturing.h"
#include "WaterRenderer.h"

DrawingManager::DrawingManager()
{
	outLine = new OutLine();
	frameBufferObj = new FrameBufferObject(nullptr, shaderFrameBufferVertex, 
		shaderFrameBufferFragment);
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
		glViewport(0, 0, Client::windowWidth, Client::windowHeight);
		ClearBuffer();
		DrawingGround(dt);
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

void DrawingManager::DrawingGround(float dt)
{
	if (Graphic::groundId != nullptr)
	{
		//Matrix mvpMat = ndcMat * camMat;
		Shader* groundShader = Graphic::groundId;
		static const GLfloat black[] = { 0.85f, 0.95f, 1.0f, 1.0f };
		static const GLfloat one = 1.0f;
		glViewport(0, 0, Client::windowWidth, Client::windowHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);
		totalTime += dt;
		float t = 0.01f * totalTime;
		float r = sinf(t * 5.37f) * 15.0f + 16.0f;
		float h = cosf(t * 4.79f) * 2.0f + 3.2f;
		vmath::mat4 mv_matrix = /* vmath::translate(0.0f, 0.0f, -1.4f) *
								vmath::translate(0.0f, -0.4f, 0.0f) * */
								// vmath::rotate((float)currentTime * 6.0f, 0.0f, 1.0f, 0.0f) *
			vmath::lookat(vmath::vec3(sinf(t) * r, h, cosf(t) * r), vmath::vec3(0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));
		vmath::mat4 proj_matrix = vmath::perspective(60.0f,
			(float)Client::windowWidth / (float)Client::windowHeight,
			0.1f, 1000.0f);
		vmath::mat4 mvp = proj_matrix * mv_matrix;
		
		groundShader->Use();

		unsigned loc = groundShader->GetUniformLocation("mv_matrix");
		unsigned loc2 = groundShader->GetUniformLocation("proj_matrix");
		unsigned loc3 = groundShader->GetUniformLocation("mvpMat");
		
		glUniformMatrix4fv(loc, 1, GL_FALSE, mv_matrix);
		glUniformMatrix4fv(loc2, 1, GL_FALSE, proj_matrix);
		glUniformMatrix4fv(loc3, 1, GL_FALSE, proj_matrix * mv_matrix);

		
		//groundShader->SendUniformMat("mvp", &mvpMat);
		//groundShader->SendUniformMat("mvpMat", &mvpMat);
		//groundShader->SendUniformMat("mv_matrix", &camMat);
		//groundShader->SendUniformMat("proj_matrix", &ndcMat);
		groundShader->SendUniformFloat("dmapDepth", 12.0f);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
		//glDrawArrays(GL_PATCHES, 0, 100);
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
