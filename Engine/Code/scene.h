#pragma once

#include "platform.h"
#include <vector>

class GameObject 
{
public:
	// model matrix / world matrix
	union {
		glm::mat4 _transform;
		struct {
			glm::vec3 _right;	double _paddingX;
			glm::vec3 _up;		double _paddingY;
			glm::vec3 _forward;	double _paddingZ;
			glm::vec3 _pos;		double _paddingW;
		};
	};

	// mesh
	u32 modelID;
	// shader
	u32 programID;
};


class Scene 
{
public:
	std::vector<GameObject> gameObjects;
};