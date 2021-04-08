#include "DrawingManager.h"
#include "Graphic.h"
#include "Projection.h"
#include <iostream>
#include "FrameBuffer.h"
#include "Skybox.h"
#include "OutLine.h"
#include "WaterFrameBuffer.h"
#include "Texture.h"
#include "vs2017/FrameBufferTexturing.h"
DrawingManager::DrawingManager()
{
	outLine = new OutLine();
	frameBufferObj = new FrameBufferObject(0);
	reflectFramebuffer = new FrameBuffer();
	refractFramebuffer = new FrameBuffer();
	checkBuffer = new FrameBuffer();
	//skyBox = new SkyBox();
	waterDuDv = new Texture("dudvMap.png");
	normalMap = new Texture("normal.png");
}

void DrawingManager::Drawing(float dt)
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	moveFactor += moveSpeed * dt;

	if(moveFactor > 1.f)
	{
		moveFactor = 0.f;
	}
	
	ndcMat = CameraToNDC(*CameraManager::instance->GetCamera());
	camMat = WorldToCamera(*CameraManager::instance->GetCamera());
	objectsSize = Graphic::objects.size();
	object = Graphic::objects;
	
	Matrix transposed = transpose(camMat);
	glBindBuffer(GL_UNIFORM_BUFFER, Graphic::instance->GetUboMatricesId());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Affine), sizeof(Affine), &transposed);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	bool isWaterExist = Graphic::water != nullptr;

	if (isWaterExist)
	{
		glEnable(GL_CLIP_DISTANCE0);
		reflectFramebuffer->Bind();
		WaterInitialize(Hcoord{ 0, 1, 0, -1.f + 0.1f });
		DrawingObjs();
		reflectFramebuffer->UnBind();

		refractFramebuffer->Bind();
		WaterInitialize(Hcoord{ 0, -1, 0, 1.f });
		DrawingObjs();
		refractFramebuffer->UnBind();

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
	DrawingWater();
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (waterObj != nullptr)
	{
		Shader* shader = waterObj->Get_Mesh()->GetShader();
		unsigned vaoId = waterObj->Get_Mesh()->Get_VAO_Id();
		Matrix model = waterObj->Get_Model_To_World();
		Point cameraPos = CameraManager::instance->GetCamera()->Eye();
		Vector3 cameraPosInVec3 = Vector3{cameraPos.x ,cameraPos.y , cameraPos.z};
		Point lightPosition = Graphic::light->Get_Obj_Pos();
		Vector3 lightPosInVec3{ lightPosition.x, lightPosition.y, lightPosition.z };
		Vector3 lightColor{1.f, 1.f, 1.f};
		shader->Use();
		glBindVertexArray(vaoId);
		shader->SendUniformMat("model", &model);
		shader->SendUniformMat("to_ndc", &ndcMat);
		shader->SendUniformMat("cam", &camMat);
		shader->SendUniformFloat("moveFactor", &moveFactor);
		shader->SendUniformVec3("cameraPosition", &cameraPosInVec3);
		shader->SendUniformVec3("lightPosition", &lightPosInVec3);
		shader->SendUniformVec3("lightColor", &lightColor);
		
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, reflectFramebuffer->TextureId());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, refractFramebuffer->TextureId());
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, waterDuDv->GetTextureId());
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalMap->GetTextureId());
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, refractFramebuffer->DepthTextureId());

		glDrawArrays(GL_TRIANGLES, 0, 3 * waterObj->Get_Mesh()->FaceCount());

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glDisable(GL_BLEND);
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

void DrawingManager::DrawLight()
{
	Object* lightObj = Graphic::light;

	if(lightObj != nullptr)
	{
		lightObj->Draw();
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
