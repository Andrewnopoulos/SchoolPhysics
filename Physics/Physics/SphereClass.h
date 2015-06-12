#pragma once

#include <glm\glm.hpp>
#include "RigidBody.h"

class SphereClass : public RigidBody
{
public:
	float m_radius;
	SphereClass(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour);
	virtual void makeGizmo();
	~SphereClass();
};

