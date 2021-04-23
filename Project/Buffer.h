#pragma once
#include <GL/glew.h>

class Buffer
{
public:
	Buffer(GLenum type, unsigned size, GLenum usage, void* data);
	void Bind(unsigned uniformBufferSlot = 0);
	unsigned GetId();
	~Buffer();

private:
	unsigned bufferId;
	GLenum type;
};

inline Buffer::Buffer(GLenum type, unsigned size, GLenum usage, void* data) : type(type)
{
	glGenBuffers(1, &bufferId);
	glBindBuffer(type, bufferId);
	glBufferData(type, size, data, usage);
}

inline void Buffer::Bind(unsigned uniformBufferSlot)
{
	switch(type)
	{
	case GL_UNIFORM_BUFFER:
		glBindBufferBase(GL_UNIFORM_BUFFER, uniformBufferSlot, bufferId);
		break;
	case GL_ARRAY_BUFFER:
		glBindBuffer(GL_ARRAY_BUFFER, bufferId);
		break;
	case GL_SHADER_STORAGE_BUFFER:
		glBindBuffer(GL_ARRAY_BUFFER, bufferId);
		break;
	default: ;
	}
}

inline unsigned Buffer::GetId()
{
	return bufferId;
}

inline Buffer::~Buffer()
{
	glDeleteBuffers(1, &bufferId);
}
