#pragma once
#include "platform.h"
#include "resources.h"
#include "framebuffer.h"

class WaterResources
{
	u32 reflectionProgramIdx;
	u32 refractionProgramIdx;

	GLuint reflectionRenderTarget = 0;
	GLuint refractionRenderTarget = 0;

	GLuint reflectionDepthRenderTarget = 0;
	GLuint refractionDepthRenderTarget = 0;

	FramebufferObject* fboReflection = nullptr;
	FramebufferObject* fboRefraction = nullptr;

	void Init(const int& screenWidth, const int& screenHeight);
};

