#include "PhysicsScene.h"
#include "RigidBody.h"
#include "SphereClass.h"
#include "Plane.h"
#include "AABBClass.h"
#include "SpringJoint.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <Gizmos.h>

PhysicsScene::PhysicsScene()
{
	Gizmos::create();
}

PhysicsScene::~PhysicsScene()
{
	Gizmos::destroy();
}

void PhysicsScene::AddActor(PhysicsObject* a_object)
{
	m_actors.push_back(a_object);
}

void PhysicsScene::RemoveActor(PhysicsObject* a_object)
{
	auto position = std::find(m_actors.begin(), m_actors.end(), a_object);
	if (position != m_actors.end())
	{
		m_actors.erase(position);
	}
}

void PhysicsScene::UpdateGizmos()
{
	Gizmos::clear();

	for each (PhysicsObject* var in m_actors)
	{
		var->MakeGizmo();
	}

	for each (Plane* plane in m_walls)
	{
		plane->MakeGizmo();
	}

	Gizmos::draw2D(glm::ortho<float>(-100, 100, -100 / AR, 100 / AR, -1.0f, 1.0f));
}

void PhysicsScene::UpdateGizmos(glm::mat4 cameraTransform)
{
	Gizmos::clear();

	for each (PhysicsObject* var in m_actors)
	{
		var->MakeGizmo();
	}

	for each (Plane* plane in m_walls)
	{
		plane->MakeGizmo();
	}

	Gizmos::draw(cameraTransform);
}

void PhysicsScene::Update()
{
	for each (PhysicsObject* var in m_actors)
	{
		var->Update(m_gravity, m_timestep);
	}
}

bool PhysicsScene::SphereSphereCollision(SphereClass* sphere1, SphereClass* sphere2)
{
	glm::vec3 delta = sphere2->m_position - sphere1->m_position;
	float distance = glm::length(delta);
	float intersection = sphere2->m_radius + sphere1->m_radius - distance;
	if (intersection > 0)
	{
		glm::vec3 collisionNormal = glm::normalize(delta);
		glm::vec3 relativeVelocity = sphere1->m_velocity - sphere2->m_velocity;
		glm::vec3 collisionVector = collisionNormal * glm::abs(glm::dot(relativeVelocity, collisionNormal));
		glm::vec3 forceVector = collisionVector * 1.0f / (1 / sphere1->m_mass + 1 / sphere2->m_mass);
		// use newton's 3rd law to apply collision forces to bodies
		sphere1->applyForceToActor(sphere2, - 2 * forceVector);
		//move spheres out of collision
		glm::vec3 separationVector = collisionNormal * intersection * 0.5f;
		sphere1->m_position -= separationVector;
		sphere2->m_position += separationVector;
		return true;
	}
	return false;
}

bool PhysicsScene::SpherePlaneCollision(SphereClass* sphere, Plane* plane)
{
	glm::vec3 collisionNormal = plane->m_normal;
	float sphereToPlane = glm::dot(sphere->m_position, plane->m_normal) - plane->m_distanceToOrigin;
	//if (sphereToPlane < 0) // if sphere is behind, flip the normal
	//{
	//	collisionNormal *= -1;
	//	sphereToPlane *= -1;
	//}
	float intersection = sphere->m_radius - sphereToPlane; // intersection between sphere and plane
	if (intersection > 0)
	{
		// find point of collision
		glm::vec3 planeNormal = plane->m_normal;
		//if (sphereToPlane < 0)
		//{
		//	planeNormal *= -1; // flip normal if behind plane
		//}
		glm::vec3 forceVector = -1 * sphere->m_mass * planeNormal * glm::dot(planeNormal, sphere->m_velocity);
		sphere->applyForce(2 * forceVector * sphere->m_elasticity);
		sphere->m_position += collisionNormal * intersection * 1.5f;
		return true;
	}
	return false;
}

// calculate all points
// dot product with normal of plane
// find closest (smallest dot product)
// compare to plane distance
// if smaller, is colliding
bool PhysicsScene::AABBPlaneCollision(AABBClass* box, Plane* plane)
{
	std::vector<glm::vec3> points;
	glm::vec3 minPoint = box->GetMin();
	glm::vec3 dim = box->m_dimensions;
	points.push_back(minPoint);
	points.push_back(minPoint + glm::vec3(dim.x, 0, 0));
	points.push_back(minPoint + glm::vec3(dim.x, dim.y, 0));
	points.push_back(minPoint + glm::vec3(dim.x, dim.y, dim.z));
	points.push_back(minPoint + glm::vec3(0, dim.y, 0));
	points.push_back(minPoint + glm::vec3(0, dim.y, dim.z));
	points.push_back(minPoint + glm::vec3(0, 0, dim.z));
	points.push_back(minPoint + glm::vec3(dim.x, 0, dim.z));

	glm::vec3 normal = plane->m_normal;

	float closest = glm::dot(minPoint, normal);

	for each (glm::vec3 var in points)
	{
		if (glm::dot(var, normal) < closest)
		{
			closest = glm::dot(var, normal);
		}
	}

	if (closest < plane->m_distanceToOrigin)
	{
		float intersection = plane->m_distanceToOrigin - closest;
		glm::vec3 forceVector = -1 * box->m_mass * normal * glm::dot(normal, box->m_velocity);
		box->applyForce(2 * forceVector * box->m_elasticity);
		box->m_position += normal * intersection;
		return true;
	}
	return false;

}

bool PhysicsScene::AABBAABBCollision(AABBClass* box1, AABBClass* box2)
{
	bool collision = false;

	glm::vec3 b1min = box1->GetMin();
	glm::vec3 b1max = box1->GetMax();
	glm::vec3 b2min = box2->GetMin();
	glm::vec3 b2max = box2->GetMax();

	collision =		b1min.x < b2max.x && b1max.x > b2min.x &&
					b1min.y < b2max.y && b1max.y > b2min.y &&
					b1min.z < b2max.z && b1max.z > b2min.z;



	return collision;
}

bool PhysicsScene::SphereAABBCollision(SphereClass* sphere, AABBClass* box)
{
	bool collision = false;
	float dist_squared = Squared(sphere->m_radius);

	glm::vec3 min = box->GetMin();
	glm::vec3 max = box->GetMax();
	glm::vec3 center = sphere->m_position;

	if (center.x < min.x) dist_squared -= Squared(center.x - min.x);
	else if (center.x > max.x) dist_squared -= Squared(center.x - max.x);

	if (center.y < min.y) dist_squared -= Squared(center.y - min.y);
	else if (center.y > max.y) dist_squared -= Squared(center.y - max.y);

	if (center.z < min.z) dist_squared -= Squared(center.z - min.z);
	else if (center.z > max.z) dist_squared -= Squared(center.z - max.z);

	collision = dist_squared > 0;

	return collision;

}
