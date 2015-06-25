#pragma once

#include <glm\glm.hpp>

#include "PhysicsObject.h"

class Plane : public PhysicsObject
{
public:
	glm::vec2 m_normal;
	float m_distanceToOrigin;

	Plane(glm::vec2 normal, float distanceToOrigin);
	virtual void Update(glm::vec2 gravity, float timeStep) {};
	virtual void Debug() {};
	virtual void MakeGizmo();
	~Plane();

};

