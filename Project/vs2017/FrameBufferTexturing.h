#pragma once

class FrameBuffer
{
public:
	FrameBuffer();
	unsigned TextureId();
	unsigned DepthTextureId();
	void Bind();
	void UnBind();
	void Use(unsigned vaoId, unsigned shaderId);
private:
	unsigned textureId;
	unsigned depthId;
	unsigned renderId;
	unsigned framebufferId;
};