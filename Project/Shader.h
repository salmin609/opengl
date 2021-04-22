#pragma once
#include <string>
#include <GL/glew.h>

class Shader 
{
public:
	Shader(const char* vertexPath, const char* fragPath);
	Shader(const char* computeShader);
	Shader(const char* vertex, const char* frag, const char* tessControl, const char* tessEval);
	unsigned Load(const char* fileName, GLenum type, bool checkError);
	unsigned GetShaderId();
	unsigned GetUniformLocation(const char* name);
	void Use();
	void SendUniformMat(std::string uniformName, void* val) const;
	void SendUniformInt(std::string uniformName, void* val) const;
	void SendUniformInt(std::string uniformName, int val) const;
	void SendUniformFloat(std::string uniformName, void* val) const;
	void SendUniformFloat(std::string uniformName, float val) const;
	void SendUniformVec3(std::string uniformName, void* val) const;
	~Shader();
private:
	unsigned programId;
	unsigned vertexShaderId;
	unsigned fragmentShaderId;
	unsigned computeShaderId;
};
