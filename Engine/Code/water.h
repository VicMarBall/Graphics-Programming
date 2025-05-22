#pragma once
#include "platform.h"
#include "resources.h"
#include "framebuffer.h"

struct WaterResources
{
	u32 reflectionProgramIdx;
	u32 refractionProgramIdx;

	GLuint rtReflection = 0;
	GLuint rtRefraction = 0;

	GLuint rtReflectionDepth = 0;
	GLuint rtRefractionDepth = 0;

	FramebufferObject fboReflection;
	FramebufferObject fboRefraction;

	void Init(const int& screenWidth, const int& screenHeight);
};

