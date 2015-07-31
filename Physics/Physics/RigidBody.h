#pragma once

#include <glm\glm.hpp>
#include "PhysicsObject.h"

class RigidBody : public PhysicsObject
{
public:
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	float m_mass;
	float m_rotation2D;
	float m_elasticity;
	bool m_static;
	RigidBody(glm::vec3 position, glm::vec3 velocity, float rotation, float mass, float elasticity);
	virtual void Update(glm::vec3 gravity, float timeStep);
	virtual void Debug() = 0;
	virtual void MakeGizmo() = 0;
	void applyForce(glm::vec3 force);
	void applyForceToActor(RigidBody* actor2, glm::vec3 force);
	~RigidBody();
};