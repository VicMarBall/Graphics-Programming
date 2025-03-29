#pragma once

#include "platform.h"
#include <vector>

class Camera 
{
public:
	Camera() { _transform = glm::identity<glm::mat4>(); }
	Camera(glm::vec3 position) 
	{
		_position = position;
	}

	glm::mat4 getTransform() const { return _transform; }

	glm::vec3 getRight() const { return _right; }
	glm::vec3 getUp() const { return _up; }
	glm::vec3 getForward() const { return _forward; }

	glm::vec3 getPosition() const { return _position; }

	void translate(glm::vec3 translation) { _transform = glm::translate(_transform, translation); }
	void rotate(float angle, glm::vec3 axis) { _transform = _transform * glm::rotate(glm::radians(angle), axis); }
	void scale(glm::vec3 scale) { _transform = glm::scale(_transform, scale); }

	float zNear = 0.01f, zFar = 1000.0f;

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

class GameObject 
{
public:
	GameObject() { _transform = glm::identity<glm::mat4>(); }
	GameObject(glm::vec3 position)
	{
		_position = position;
	}

	// mesh
	u32 modelID;
	// shader
	u32 programID;

	glm::mat4 getTransform() const { return _transform; }

	glm::vec3 getRight() const { return _right; }
	glm::vec3 getUp() const { return _up; }
	glm::vec3 getForward() const { return _forward; }

	glm::vec3 getPosition() const { return _position; }

	void translate(glm::vec3 translation) { _transform = glm::translate(_transform, translation); }
	void rotate(float angle, glm::vec3 axis) { _transform = _transform * glm::rotate(glm::radians(angle), axis); }
	void scale(glm::vec3 scale) { _transform = glm::scale(_transform, scale); }

private:
	// model matrix / world matrix
	union {
		glm::mat4 _transform;
		struct {
			glm::vec3 _right;	float _paddingX;
			glm::vec3 _up;		float _paddingY;
			glm::vec3 _forward;	float _paddingZ;
			glm::vec3 _position;float _paddingW;
		};
	};

};


class Scene 
{
public:
	Camera camera;
	std::vector<GameObject> gameObjects;
};