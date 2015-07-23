#include "RigidBody.h"

RigidBody::~RigidBody()
{

}

RigidBody::RigidBody(glm::vec3 position, glm::vec3 velocity, float rotation, float mass)
{
	m_position = position;
	m_velocity = velocity;
	m_rotation2D = rotation;
	m_mass = mass;
}

void RigidBody::applyForce(glm::vec3 force)
{
	m_velocity += force / m_mass;
}

void RigidBody::applyForceToActor(RigidBody* actor2, glm::vec3 force)
{
	actor2->applyForce(-force);
	applyForce(force);
}

void RigidBody::Update(glm::vec3 gravity, float timeStep)
{
	m_velocity += gravity * timeStep;
	m_position += m_velocity * timeStep;
}