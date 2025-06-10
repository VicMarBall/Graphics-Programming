#pragma once
#include "platform.h"
#include "resources.h"
#include "framebuffer.h"
#include "game_object.h"

enum WaterScenePart
{
	REFLECTION, 
	REFRACTION
};

struct WaterResources
{
	u32 waterProgramIdx;
	u32 forwardClipTexturedMeshProgramIdx;
	u32 forwardClipBasicShapesProgramIdx;

	GLuint rtReflection = 0;
	GLuint rtRefraction = 0;

	GLuint rtReflectionDepth = 0;
	GLuint rtRefractionDepth = 0;

	FramebufferObject fboReflection;
	FramebufferObject fboRefraction;

	GameObject waterObj;

	void Init(u32 planeIdx, const int& screenWidth, const int& screenHeight);
};