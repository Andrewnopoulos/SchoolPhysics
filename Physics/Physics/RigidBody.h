#pragma once

#include <glm\glm.hpp>

class RigidBody
{
public:
	glm::vec2 m_position;
	glm::vec2 m_velocity;
	float m_mass;
	float m_rotation2D;
	RigidBody(glm::vec2 position, glm::vec2 velocity, float rotation, float mass);
	virtual void Update(glm::vec2 gravity, float timeStep);
	virtual void debug();
	void applyForce(glm::vec2 force);
	void applyForceToActor(RigidBody* actor2, glm::vec2 force);
	~RigidBody();
};

