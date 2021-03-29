#include "FrameBuffer.h"
#include "Graphic.h"
#include <iostream>

FrameBufferObject::FrameBufferObject()
{
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVao);
	glGenBuffers(1, &quadVbo);
	glBindVertexArray(quadVao);
	glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	shader = new Shader(shaderFrameBufferVertex.c_str(),
		shaderToonifyPostProcessFragment.c_str());

	int val = 0;
	float val2 = 0.2f;
	float val3 = 5.0f;
	float val4 = 0.f;
	shader->Use();
	shader->SendUniformInt("tex0", &val);
	shader->SendUniformFloat("edge_thres", &val2);
	shader->SendUniformFloat("mouse_x_offset", &val4);
	shader->SendUniformFloat("edge_thres2", &val3);

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

	glGenRenderbuffers(1, &renderBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER, renderBufferId);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error ! frame buffer is not complete" << std::endl;
	}
	else if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "frame buffer is complete" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void FrameBufferObject::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
}

void FrameBufferObject::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::Use()
{
	shader->Use();
	glBindVertexArray(quadVao);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, textureColorBufferId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
