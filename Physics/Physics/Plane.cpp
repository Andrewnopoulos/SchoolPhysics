#include "Plane.h"
#include <Gizmos.h>

Plane::Plane(glm::vec3 normal, float distanceToOrigin)
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
	glm::vec3 point1 = (m_normal * m_distanceToOrigin) + (glm::vec3(m_normal.y, -m_normal.x, 0) * 100.0f);
	glm::vec3 point2 = (m_normal * m_distanceToOrigin) + (glm::vec3(-m_normal.y, m_normal.x, 0) * 100.0f);
	Gizmos::addLine(point1, point2, glm::vec4(1, 1, 1, 1));
}