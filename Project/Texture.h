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
	Texture(GLenum internalFormat = GL_RGBA);
	Texture(const std::string& fileName, bool hasTransparency = false, GLenum internalFormat = GL_RGBA);
	Texture(const std::string& fileName, GLenum internalFormat, int w, int h);
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
