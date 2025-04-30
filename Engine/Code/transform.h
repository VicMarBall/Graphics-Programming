#pragma once

#include "platform.h"

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
	glm::vec3 getRotation()
	{
		glm::quat quaternion = glm::quat_cast((glm::mat3)_transform);

		glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(quaternion));
		eulerAngles.x *= (1.0f / getScale().x);
		eulerAngles.y *= (1.0f / getScale().y);
		eulerAngles.z *= (1.0f / getScale().z);

		return eulerAngles;
	}
	glm::vec3 getScale() { return glm::vec3(glm::length(_right), glm::length(_up), glm::length(_forward)); }

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

	void setPosition(glm::vec3 position)
	{
		_position = position;
	}

	void setRotation(glm::vec3 rotation)
	{
		rotation = glm::radians(rotation);

		glm::quat quaternion = glm::angleAxis(rotation.x, glm::vec3(1, 0, 0));
		quaternion *= glm::angleAxis(rotation.y, glm::vec3(0, 1, 0));
		quaternion *= glm::angleAxis(rotation.z, glm::vec3(0, 0, 1));

		_transform = glm::translate(glm::mat4(1.0f), getPosition()) * glm::mat4_cast(quaternion) * glm::scale(glm::mat4(1.0f), getScale());
	}

	void setScale(glm::vec3 scale)
	{
		if (scale.x <= 0 || scale.y <= 0 || scale.z <= 0) {
			return;
		}
		_right = glm::normalize(_right) * scale.x;
		_up = glm::normalize(_up) * scale.y;
		_forward = glm::normalize(_forward) * scale.z;
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
