#include "Texture.h"

#include "Client.h"
#include "Image.h"
Texture::Texture(GLenum internalFormat)
{
	width = Client::windowWidth;
	height = Client::windowHeight;
	glGenTextures(1, &textureID);
	pixelData = nullptr;
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
	Unbind();
}
Texture::Texture(const std::string& fileName, bool hasTransparency, GLenum internalFormat) : fileName(fileName)
{
	Image tempImage;
	pixelData = tempImage.Load_Image(fileName, width, height, true);
	this->hasTransparency = hasTransparency;
	glGenTextures(1, &textureID);
	Bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

	glGenerateMipmap(GL_TEXTURE_2D);
	Unbind();
}

Texture::Texture(const std::string& fileName, GLenum internalFormat, int w, int h)
{
	(w);
	(h);
	Image tempImage;
	pixelData = tempImage.Load_Image(fileName, width, height, true);
	glGenTextures(1, &textureID);
	Bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

	glGenerateMipmap(GL_TEXTURE_2D);
	Unbind();
}

Texture::~Texture() {
	glDeleteTextures(1, &textureID);
}

void Texture::GetImageWidthHeight(int& imgWidth, int& imgHeight)
{
	imgWidth = this->width;
	imgHeight = this->height;
}

bool Texture::HasTransparency() {
	return hasTransparency;
}

void Texture::BindImageTexture(int index, GLenum type)
{
	Bind();
	glBindImageTexture(index , textureID, 0, GL_FALSE, 0, type, GL_RGBA32F);
	Unbind();
}

void Texture::Bind(int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned Texture::GetTextureId()
{
	return textureID;
}

Texture* Texture::CreateTextureAttachment(int width, int height) {
	Texture* texture = new Texture();
	texture->width = width, texture->height = height;
	texture->hasTransparency = false;

	glGenTextures(1, &(texture->textureID));
	glBindTexture(GL_TEXTURE_2D, texture->textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->textureID, 0);

	return texture;
}
