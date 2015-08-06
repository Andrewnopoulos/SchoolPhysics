#include "SpringJoint.h"
#include "RigidBody.h"
#include "Gizmos.h"


SpringJoint::SpringJoint(RigidBody* connection1, RigidBody* connection2, float springCoefficient, float damping)
{
	m_connections[0] = connection1;
	m_connections[1] = connection2;
	m_springCoefficient = springCoefficient;
	m_damping = damping;
	m_restLength = (m_connections[0]->m_position - m_connections[1]->m_position);
	_shapeID = JOINT;
}


SpringJoint::~SpringJoint()
{
}

void SpringJoint::MakeGizmo()
{
	Gizmos::addTri(m_connections[0]->m_position, 
		m_connections[1]->m_position, 
		m_connections[0]->m_position + glm::vec3(0.2f, 0.2f, 0.2f),
		glm::vec4(1, 0, 1, 1));
	Gizmos::addTri(m_connections[1]->m_position,
		m_connections[1]->m_position + glm::vec3(0.2f, 0.2f, 0.2f),
		m_connections[0]->m_position + glm::vec3(0.2f, 0.2f, 0.2f),
		glm::vec4(1, 0, 1, 1));
}

void SpringJoint::Update(glm::vec3 gravity, float timestep)
{
	glm::vec3 dir = m_connections[0]->m_position - m_connections[1]->m_position;
	glm::vec3 rel = m_connections[0]->GetVelocity() - m_connections[1]->GetVelocity();

	//float K = m_springCoefficient;
	//
	//float X = glm::length(dir) - m_restLength;

	//float b = m_damping;

	//float v = glm::length(rel);

	//float force = (-K * X);

	//glm::vec3 forceDir = glm::normalize(dir);

	glm::vec3 force = -m_springCoefficient * (dir - m_restLength);
	force -= m_damping * rel;

	if (m_connections[0]->m_static)
	{
		m_connections[1]->applyForce(-0.5f * force);
	}
	else if (m_connections[1]->m_static)
	{
		m_connections[0]->applyForce(0.5f * force);
	}
	else
	{
		m_connections[0]->applyForceToActor(m_connections[1], force);
	}

}