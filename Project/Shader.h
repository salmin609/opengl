#pragma once

class Shader 
{
public:
	Shader(const char* vertexPath, const char* fragPath);
	unsigned GetShaderId();

private:
	unsigned shaderId;
};