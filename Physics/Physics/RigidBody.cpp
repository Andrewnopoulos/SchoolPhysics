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
}

void RigidBody::applyForce(glm::vec3 force)
{
	m_velocity += force / m_mass;
}

void RigidBody::applyForceToActor(RigidBody* actor2, glm::vec3 force)
{
	actor2->applyForce(-force * actor2->m_elasticity);
	applyForce(force * m_elasticity);
}

void RigidBody::Update(glm::vec3 gravity, float timeStep)
{
	m_velocity += gravity * timeStep;
	m_position += m_velocity * timeStep;
}