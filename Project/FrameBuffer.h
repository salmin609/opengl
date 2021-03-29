#pragma once

class Shader;

class FrameBufferObject
{
public:
	FrameBufferObject();
	void Bind() const;
	void UnBind();
	void Use();
private:
	unsigned quadVao;
	unsigned quadVbo;

	unsigned frameBufferId;
	unsigned textureColorBufferId;
	unsigned renderBufferId;
	Shader* shader;
};
