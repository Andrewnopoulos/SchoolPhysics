#include "AABBClass.h"
#include <Gizmos.h>


AABBClass::AABBClass(glm::vec3 position, glm::vec3 velocity, float mass, glm::vec3 dimensions, glm::vec4 colour)
: RigidBody(position, velocity, 0, mass)
{
	m_dimensions = dimensions;
	m_colour = colour;
	_shapeID = AABB;
}

AABBClass::~AABBClass()
{
}

void AABBClass::MakeGizmo()
{
	Gizmos::addAABBFilled(m_position + (m_dimensions)/2.0f, m_dimensions / 2.0f, m_colour);
}

void AABBClass::SetPosition(glm::vec3 a_position)
{
	m_position = a_position;
}

void AABBClass::Update(glm::vec3 gravity, float timeStep)
{
	m_position += m_velocity * timeStep;
}