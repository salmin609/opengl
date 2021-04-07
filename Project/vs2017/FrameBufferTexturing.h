#pragma once

class FrameBuffer
{
public:
	FrameBuffer();
	unsigned TextureId();
	unsigned DepthTextureId();
	void Bind();
	void UnBind();
private:
	unsigned textureId;
	unsigned depthId;
	unsigned renderId;
	unsigned framebufferId;
};