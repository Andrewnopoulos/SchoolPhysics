#include "SphereClass.h"


SphereClass::SphereClass(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour) : RigidBody(position, velocity, 0, mass)
{
	m_radius = radius;
	m_colour = colour;
}


SphereClass::~SphereClass()
{
}
