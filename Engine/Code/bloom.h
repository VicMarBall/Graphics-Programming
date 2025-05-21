#pragma once
#include "platform.h"
#include "resources.h"
#include "framebuffer.h"

struct BloomResources
{
	u32 blitBrightestPixelsProgramIdx;
	u32 blurProgramIdx;
	u32 bloomProgramIdx;

	// bloom mipmap
	GLuint rtBright;
	GLuint rtBloomH;
	FramebufferObject fboBloom1; 
	FramebufferObject fboBloom2; 
	FramebufferObject fboBloom3; 
	FramebufferObject fboBloom4; 
	FramebufferObject fboBloom5; 

	void Init(const int& screenWidth, const int& screenHeight);

	// parameters
	float threshold = 1.0f;
	float intensity = 1.0f;
};