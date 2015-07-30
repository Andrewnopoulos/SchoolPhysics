#include "SpringJoint.h"
#include "RigidBody.h"
#include "Gizmos.h"


SpringJoint::SpringJoint(RigidBody* connection1, RigidBody* connection2, float springCoefficient, float damping)
{
	m_connections[0] = connection1;
	m_connections[1] = connection2;
	m_springCoefficient = springCoefficient;
	m_damping = damping;
	m_restLength = glm::length(m_connections[0]->m_position - m_connections[1]->m_position);
	_shapeID = JOINT;
}


SpringJoint::~SpringJoint()
{
}

void SpringJoint::MakeGizmo()
{
	Gizmos::addTri(m_connections[0]->m_position, 
		m_connections[1]->m_position, 
		m_connections[0]->m_position + glm::vec3(2, 2, 2), 
		glm::vec4(1, 0, 1, 1));
	Gizmos::addTri(m_connections[1]->m_position,
		m_connections[1]->m_position + glm::vec3(2, 2, 2),
		m_connections[0]->m_position + glm::vec3(2, 2, 2),
		glm::vec4(1, 0, 1, 1));
}

void SpringJoint::Update(glm::vec3 gravity, float timestep)
{
	glm::vec3 dir = m_connections[0]->m_position - m_connections[1]->m_position;
	float X = glm::length(dir) - m_restLength;
	m_connections[0]->applyForceToActor(m_connections[1], -1.0f * m_springCoefficient * X * dir);
}