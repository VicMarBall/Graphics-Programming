#pragma once

#include "platform.h"
#include "transform.h"

enum LightType
{
	LightType_Point,
	LightType_Directional
};

struct Light
{
	LightType type;
	glm::vec3 color;

	Transform transform;

	u32 localUniformBufferHead;
	u32 localUniformBufferSize;
};
