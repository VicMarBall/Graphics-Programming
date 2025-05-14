#pragma once
#include "platform.h"
#include "resources.h"
#include "framebuffer.h"

struct BloomResources
{
	Program* blitBrightestPixelsProgram = nullptr;
	Program* blurProgram = nullptr;
	Program* bloomProgram = nullptr;

	// bloom mipmap
	GLuint rtBright;
	GLuint rtBloomH;
	FramebufferObject fboBloom1; 
	FramebufferObject fboBloom2; 
	FramebufferObject fboBloom3; 
	FramebufferObject fboBloom4; 
	FramebufferObject fboBloom5; 

	void Init(const int& screenWidth, const int& screenHeight);
};