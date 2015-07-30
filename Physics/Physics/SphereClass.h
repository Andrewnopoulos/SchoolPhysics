#pragma once

#include <glm\glm.hpp>
#include "RigidBody.h"

class SphereClass : public RigidBody
{
public:
	float m_radius;
	glm::vec4 m_colour;

	SphereClass(glm::vec3 position, glm::vec3 velocity, float mass, float radius, float elasticity, glm::vec4 colour);
	virtual void Debug();
	virtual void MakeGizmo();
	~SphereClass();
};