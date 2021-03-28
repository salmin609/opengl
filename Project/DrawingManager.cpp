#include "DrawingManager.h"
#include "Graphic.h"
#include "Projection.h"
#include <iostream>

DrawingManager::DrawingManager()
{
	simpleShader = new Shader(shader_object_vertex.c_str(),
		shaderSingleColorFragment.c_str());
	screenShader = new Shader(shaderFrameBufferVertex.c_str(), 
		shaderFrameBufferFragment.c_str());

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	int val = 0;
	screenShader->Use();
	screenShader->SendUniformInt("screenTextures", &val);


	
	glGenFramebuffers(1, &frameBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

	glGenTextures(1, &textureColorBufferId);
	glBindTexture(GL_TEXTURE_2D, textureColorBufferId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 600, 600
		, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, textureColorBufferId, 0);

	glGenRenderbuffers(1, &renderbufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER, renderbufferId);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error ! frame buffer is not complete" << std::endl;
	}
	else if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "frame buffer is complete" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	

}

void DrawingManager::Drawing()
{
	ndcMat = CameraToNDC(*CameraManager::instance->GetCamera());
	camMat = WorldToCamera(*CameraManager::instance->GetCamera());
	objectsSize = Graphic::objects.size();
	object = Graphic::objects;

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	glEnable(GL_DEPTH_TEST);
	DrawingGround();
	DrawingShadow();
	OutlinePrepare();
	DrawingObjs();
	DrawingOutline();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	//glClearColor(1.f, 1.f, 1.f, 1.f);
	screenShader->Use();
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorBufferId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
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
		simpleShader->Use();
		simpleShader->SendUniformMat("to_ndc", &ndcMat);
		simpleShader->SendUniformMat("cam", &camMat);

		selectedObj->SetVAO();

		Matrix model2World = selectedObj->GetModelToWorldOutlineScaling(0.1f);
		Mesh* mesh = selectedObj->Get_Mesh();
		simpleShader->SendUniformMat("model", &model2World);

		
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
