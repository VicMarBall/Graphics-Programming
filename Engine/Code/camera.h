#pragma once

#include "platform.h"
#include "transform.h"

class Camera
{
public:
	Camera() {}
	Camera(glm::vec3 position)
	{
		transform = Transform(position);
	}

	Transform transform;

	float zNear = 0.01f, zFar = 1000.0f;
};