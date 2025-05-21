#include "water.h"

void WaterResources::Init(const int& screenWidth, const int& screenHeight)
{
	glGenTextures(1, &reflectionRenderTarget);
	glBindTexture(GL_TEXTURE_2D, reflectionRenderTarget);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glGenTextures(1, &refractionRenderTarget);
	glBindTexture(GL_TEXTURE_2D, refractionRenderTarget);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);


	glGenTextures(1, &reflectionDepthRenderTarget);
	glBindTexture(GL_TEXTURE_2D, reflectionRenderTarget);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glGenTextures(1, &refractionDepthRenderTarget);
	glBindTexture(GL_TEXTURE_2D, refractionRenderTarget);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);


	fboReflection->bind();
	fboReflection->addColorAttachment(GL_DEPTH_ATTACHMENT, reflectionDepthRenderTarget);
	fboReflection->addColorAttachment(GL_COLOR_ATTACHMENT0, reflectionRenderTarget, 0);
	fboReflection->checkStatus();
	fboReflection->unbind();

	fboRefraction->bind();
	fboRefraction->addColorAttachment(GL_DEPTH_ATTACHMENT, refractionDepthRenderTarget);
	fboRefraction->addColorAttachment(GL_COLOR_ATTACHMENT0, refractionRenderTarget, 0);
	fboRefraction->checkStatus();
	fboRefraction->unbind();
}