#pragma once
#include <string>

class Shader 
{
public:
	Shader(const char* vertexPath, const char* fragPath);
	Shader(const char* computeShader);
	unsigned GetShaderId();
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
