#include "FrameBuffer.h"
#include "Graphic.h"
#include <iostream>

FrameBufferObject::FrameBufferObject(int colorSlot, int width, int height)
{
	colorAttachmentSlot = colorSlot;
	colorAttachmentSlot = 0;
	fboWidth = width;
	fboHeight = height;
	
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
		shaderFrameBufferFragment.c_str());

	int val = 0;
	float val2 = 0.2f;
	float val3 = 5.0f;
	float val4 = 0.5f;
	shader->Use();
	shader->SendUniformInt("tex0", &val);
	shader->SendUniformFloat("edge_thres", &val2);
	shader->SendUniformFloat("mouse_x_offset", &val4);
	shader->SendUniformFloat("edge_thres2", &val3);

	glGenFramebuffers(1, &frameBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

	glGenTextures(1, &textureColorBufferId);
	glBindTexture(GL_TEXTURE_2D, textureColorBufferId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboWidth, fboHeight
		, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentSlot,
		GL_TEXTURE_2D, textureColorBufferId, 0);

	glGenRenderbuffers(1, &renderBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);
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
	//UseFrameBuffer(fboSrc);
	glBindVertexArray(quadVao);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, textureColorBufferId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

unsigned FrameBufferObject::GetFrameBufferId()
{
	return frameBufferId;
}

unsigned FrameBufferObject::GetTextureColorBufferId()
{
	return textureColorBufferId;
}

void FrameBufferObject::UseFrameBuffer(FrameBufferObject* fboSrc, int srcX, int srcY, int destX, int destY)
{
	if (fboSrc != nullptr)
	{
		unsigned srcWidth, srcHeight;
		
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboSrc->GetFrameBufferId());
		glReadBuffer(fboSrc->GetFrameBufferId());
		fboSrc->GetFboWidthHeight(srcWidth, srcHeight);
		glBlitFramebuffer(0, 0, srcWidth, srcHeight, srcX, srcY, destX, destY,
			GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

unsigned FrameBufferObject::ColorAttachMentSlot()
{
	return colorAttachmentSlot;
}

void FrameBufferObject::GetFboWidthHeight(unsigned& width, unsigned& height)
{
	width = fboWidth;
	height = fboHeight;
}

void FrameBufferObject::SetViewPort()
{
	int window_viewport_width = 600;
	int window_viewport_height = 600;
	
	int x = -(window_viewport_width - static_cast<int>(fboWidth)) / 2;
	int y = -(window_viewport_height - static_cast<int>(fboHeight)) / 2;

	glViewport(x, y, window_viewport_width, window_viewport_height);
}

void FrameBufferObject::ResetViewPort()
{
	glViewport(0, 0, 600, 600);
}

FrameBufferObject::~FrameBufferObject()
{
	glDeleteFramebuffers(1, &frameBufferId);
}
