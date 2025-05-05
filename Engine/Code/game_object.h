#pragma once

#include "platform.h"
#include "transform.h"

class GameObject
{
public:
	GameObject() {}
	GameObject(glm::vec3 position)
	{
		transform = Transform(position);
	}

	// mesh
	u32 modelID;
	// shader
	u32 programID;

	Transform transform;

	u32 localUniformBufferHead;
	u32 localUniformBufferSize;
};
