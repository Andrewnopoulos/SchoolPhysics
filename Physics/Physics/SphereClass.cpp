#include "SphereClass.h"
#include <Gizmos.h>

SphereClass::SphereClass(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour) : RigidBody(position, velocity, 0, mass)
{
	m_radius = radius;
	m_colour = colour;
}

SphereClass::~SphereClass()
{
}

void SphereClass::MakeGizmo()
{
	Gizmos::addSphere(glm::vec3(m_position, 0), m_radius, 20, 20, m_colour);
}

void SphereClass::Debug()
{

}