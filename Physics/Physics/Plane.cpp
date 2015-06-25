#include "Plane.h"
#include <Gizmos.h>

Plane::Plane(glm::vec2 normal, float distanceToOrigin)
{
	m_normal = glm::normalize(normal);
	m_distanceToOrigin = distanceToOrigin;
	_shapeID = PLANE;
}


Plane::~Plane()
{
}

void Plane::MakeGizmo()
{
	glm::vec2 point1 = (m_normal * m_distanceToOrigin) + (glm::vec2(m_normal.y, -m_normal.x) * 100.0f);
	glm::vec2 point2 = (m_normal * m_distanceToOrigin) + (glm::vec2(-m_normal.y, m_normal.x) * 100.0f);
	Gizmos::addLine(glm::vec3(point1, 0), glm::vec3(point2, 0), glm::vec4(1, 1, 1, 1));
}