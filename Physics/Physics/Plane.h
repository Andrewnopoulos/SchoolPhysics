#pragma once

#include <glm\glm.hpp>

#include "PhysicsObject.h"

class Plane : public PhysicsObject
{
public:
	glm::vec3 m_normal;
	float m_distanceToOrigin;

	Plane(glm::vec3 normal, float distanceToOrigin);
	virtual void Update(glm::vec3 gravity, float timeStep) {};
	virtual void Debug() {};
	virtual void MakeGizmo();
	~Plane();

};

