#pragma once

#include "platform.h"

enum TransformOrientation {
	LOCAL,
	GLOBAL
};

class Transform
{
public:
	Transform() 
	{ 
		_position = glm::vec3(0, 0, 0);
		_rotation = glm::vec3(0, 0, 0);
		_scale = glm::vec3(1, 1, 1);
	}
	Transform(glm::vec3 position)
	{
		_position = position;
		_rotation = glm::vec3(0, 0, 0);
		_scale = glm::vec3(1, 1, 1);
	}

	glm::vec3 getPosition() const { return _position; }
	glm::vec3 getRotation() const {	return _rotation; }
	glm::vec3 getScale() const { return _scale; }

	void setPosition(glm::vec3 position)
	{
		_position = position;
	}

	void setRotation(glm::vec3 rotation)
	{
		_rotation = rotation;
	}

	void setScale(glm::vec3 scale)
	{
		_scale = scale;
	}

	glm::mat4 getTransformationMatrix() 
	{
		return glm::translate(glm::mat4(1.0f), _position) 
			* glm::mat4_cast(glm::quat(glm::radians(_rotation))) 
			* glm::scale(glm::mat4(1.0f), _scale);
	}

private:
	glm::vec3 _position;
	glm::vec3 _rotation; // degrees
	glm::vec3 _scale;

};
