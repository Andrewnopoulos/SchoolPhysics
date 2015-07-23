#include "SphereClass.h"
#include <Gizmos.h>

SphereClass::SphereClass(glm::vec3 position, glm::vec3 velocity, float mass, float radius, glm::vec4 colour) : RigidBody(position, velocity, 0, mass)
{
	m_radius = radius;
	m_colour = colour;
	_shapeID = SPHERE;
}

SphereClass::~SphereClass()
{
}

void SphereClass::MakeGizmo()
{
	Gizmos::addSphere(m_position, m_radius, 20, 20, m_colour);
}

void SphereClass::Debug()
{

}