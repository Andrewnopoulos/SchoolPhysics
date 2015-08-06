#pragma once
#include "PhysicsObject.h"

class RigidBody;

class SpringJoint :
	public PhysicsObject
{
private:
	virtual void Update(glm::vec3 gravity, float timestep);
	virtual void Debug(){};
	virtual void MakeGizmo();

	RigidBody* m_connections[2];
	float m_damping;
	glm::vec3 m_restLength;
	float m_springCoefficient;

public:
	SpringJoint(RigidBody* connection1, RigidBody* connection2, float springCoefficient, float damping);
	~SpringJoint();
};