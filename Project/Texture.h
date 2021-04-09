#pragma once
#include <string>
#include <GL\glew.h>

class Texture {
private:
	std::string fileName;
	unsigned textureID;
	GLsizei width, height;
	bool hasTransparency;

public:
	Texture();
	Texture(const std::string& fileName, bool hasTransparency = false);
	~Texture();
	void GetImageWidthHeight(int& imgWidth, int& imgHeight);
	bool HasTransparency();
	void BindImageTexture(int index, GLenum type);
	void Bind(int index = 0);
	void Unbind();
	unsigned GetTextureId();
	static Texture* CreateTextureAttachment(int width, int height);
	unsigned char* pixelData;
};
