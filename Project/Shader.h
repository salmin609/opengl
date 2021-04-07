#pragma once
#include <string>

class Shader 
{
public:
	Shader(const char* vertexPath, const char* fragPath);
	unsigned GetShaderId();
	void Use();
	void SendUniformMat(std::string uniformName, void* val) const;
	void SendUniformInt(std::string uniformName, void* val) const;
	void SendUniformInt(std::string uniformName, int val) const;
	void SendUniformFloat(std::string uniformName, void* val) const;
	void SendUniformFloat(std::string uniformName, float val) const;
	void SendUniformVec3(std::string uniformName, void* val) const;
private:
	unsigned shaderId;
};
