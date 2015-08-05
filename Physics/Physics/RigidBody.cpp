#include "RigidBody.h"

RigidBody::~RigidBody()
{

}

RigidBody::RigidBody(glm::vec3 position, glm::vec3 velocity, float rotation, float mass, float elasticity)
{
	m_position = position;
	m_velocity = velocity;
	m_rotation2D = rotation;
	m_mass = mass;
	m_elasticity = elasticity;
	m_static = false;
}

void RigidBody::applyForce(glm::vec3 force)
{
	glm::vec3 outforce = force * m_elasticity;
	m_velocity += outforce / m_mass;
}

void RigidBody::applyForceToActor(RigidBody* actor2, glm::vec3 force)
{
	actor2->applyForce(-force);
	applyForce(force);
}

void RigidBody::Update(glm::vec3 gravity, float timeStep)
{
	if (!m_static)
	{
		m_velocity += gravity * timeStep;
		m_position += m_velocity * timeStep;
	}
	else
	{
		m_velocity = glm::vec3(0);
	}
}

glm::vec3 RigidBody::GetVelocity()
{
	if (m_static)
	{
		return glm::vec3(0);
	}

	return m_velocity;
}