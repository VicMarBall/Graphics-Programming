#pragma once

#include "platform.h"
#include <vector>

class Camera 
{
public:
	Camera() { }
	Camera(glm::vec3 position) 
	{
		_pos = position;
	}

	glm::mat4 getTransform() { return _transform; }

	union {
		glm::mat4 _transform;
		struct {
			glm::vec3 _right;	double _paddingX;
			glm::vec3 _up;		double _paddingY;
			glm::vec3 _forward;	double _paddingZ;
			glm::vec3 _pos;		double _paddingW;
		};
	};
};

class GameObject 
{
public:
	GameObject() { }
	GameObject(glm::vec3 position)
	{
		_pos = position;
	}

	glm::mat4 getTransform() { return _transform; }

	// mesh
	u32 modelID;
	// shader
	u32 programID;

private:
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

};


class Scene 
{
public:
	Camera camera;
	std::vector<GameObject> gameObjects;
};