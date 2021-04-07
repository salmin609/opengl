#pragma once

class Shader;

class FrameBufferObject
{
public:
	FrameBufferObject(int colorSlot, int width = 1024, int height = 768);
	void Bind() const;
	void UnBind();
	void Use();
	unsigned GetFrameBufferId();
	unsigned GetTextureColorBufferId();
	void UseFrameBuffer(FrameBufferObject* fboSrc, int srcX, int srcY, int destX, int destY);
	void UseFrameBuffer(unsigned froSrc, int srcX, int srcY, int destX, int destY);
	unsigned ColorAttachMentSlot();
	void GetFboWidthHeight(unsigned& width, unsigned& height);
	void SetViewPort();
	void ResetViewPort();
	~FrameBufferObject();
private:
	unsigned quadVao;
	unsigned quadVbo;

	unsigned frameBufferId;
	unsigned textureColorBufferId;
	unsigned renderBufferId;
	unsigned colorAttachmentSlot;
	Shader* shader;
	unsigned fboWidth;
	unsigned fboHeight;
};
