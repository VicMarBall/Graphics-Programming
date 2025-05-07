#pragma once
#include "platform.h"
#include "resources.h"

struct BloomResources
{
	Program* blitBrightestPixelsProgram = nullptr;
	Program* blurProgram = nullptr;
	Program* bloomProgram = nullptr;

	// bloom mipmap
	GLuint rtBright;
	GLuint rtBloomH;
	//to implement FramebufferObject as struct/class
	//FramebufferObject* fboBloom1 = nullptr; 
	//FramebufferObject* fboBloom2 = nullptr; 
	//FramebufferObject* fboBloom3 = nullptr; 
	//FramebufferObject* fboBloom4 = nullptr; 
	//FramebufferObject* fboBloom5 = nullptr; 
};