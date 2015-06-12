#pragma once

#include <glm\glm.hpp>

enum ShapeType
{
	PLANE = 0,
	SPHERE = 1,
	BOX = 2
};

class PhysicsObject
{
public:
	ShapeType _shapeID;
	virtual void Update(glm::vec3 gravity, float timeStep) = 0;
	virtual void Debug() = 0;
	virtual void MakeGizmo() = 0;
	virtual void ResetPosition(){};
};


