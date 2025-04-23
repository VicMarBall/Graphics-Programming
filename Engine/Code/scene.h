#pragma once

#include "platform.h"
#include <vector>

enum TransformOrientation {
	LOCAL,
	GLOBAL
};

class Transform 
{
public:
	Transform() { _transform = glm::identity<glm::mat4>(); }
	Transform(glm::vec3 position)
	{
		_position = position;
	}

	glm::mat4 getTransform() const { return _transform; }

	glm::vec3 getRight() const { return _right; }
	glm::vec3 getUp() const { return _up; }
	glm::vec3 getForward() const { return _forward; }

	glm::vec3 getPosition() const { return _position; }

	void translate(glm::vec3 translation, TransformOrientation orientation = LOCAL)
	{
		switch (orientation)
		{
		case LOCAL:
			_transform = glm::translate(_transform, translation);
			break;
		case GLOBAL:
			_position += translation;
			break;
		}
	}
	void rotate(float degrees, glm::vec3 axis, TransformOrientation orientation = LOCAL)
	{
		switch (orientation)
		{
		case LOCAL:
			_transform = glm::rotate(_transform, glm::radians(degrees), axis);
			break;
		case GLOBAL:
			_transform = glm::rotate(_transform, glm::radians(degrees), axis * (glm::mat3)_transform);
			break;
		}
	}
	void scale(glm::vec3 scale, TransformOrientation orientation = LOCAL)
	{
		switch (orientation)
		{
		case LOCAL:
			_transform = glm::scale(_transform, scale);
			break;
		case GLOBAL:
			_transform = glm::scale(_transform, scale);
			break;
		}
	}

private:
	union {
		glm::mat4 _transform;
		struct {
			glm::vec3 _right;		float _paddingX;
			glm::vec3 _up;			float _paddingY;
			glm::vec3 _forward;		float _paddingZ;
			glm::vec3 _position;	float _paddingW;
		};
	};
};

class Camera 
{
public:
	Camera() { }
	Camera(glm::vec3 position) 
	{
		transform = Transform(position);
	}

	Transform transform;

	float zNear = 0.01f, zFar = 1000.0f;
};

class GameObject 
{
public:
	GameObject() { }
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

class Scene 
{
public:
	Camera camera;
	std::vector<GameObject> gameObjects;
	std::vector<Light> lights;
};