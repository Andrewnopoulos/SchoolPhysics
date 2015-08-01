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

void PhysicsScene::CollisionResponse(glm::vec3 collisionNormal, RigidBody* object1, RigidBody* object2)
{
	glm::vec3 relativeVelocity = object1->m_velocity - object2->m_velocity;
	glm::vec3 collisionVector = collisionNormal * glm::abs(glm::dot(relativeVelocity, collisionNormal));
	glm::vec3 forceVector = collisionVector * 1.0f / (1 / object1->m_mass + 1 / object2->m_mass);
	// use newton's 3rd law to apply collision forces to bodies
	
	if (object1->m_static)
	{
		object2->applyForce(forceVector);
	}
	else if (object2->m_static)
	{
		object1->applyForce(-1 * forceVector);
	}
	else
	{
		object1->applyForceToActor(object2, -2 * forceVector);
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
		CollisionResponse(collisionNormal, sphere1, sphere2);

		//move spheres out of collision
		glm::vec3 separationVector = collisionNormal * intersection * 0.5f;

		if (sphere1->m_static)
		{
			sphere2->m_position += separationVector * 2;
		}
		else if (sphere2->m_static)
		{
			sphere1->m_position -= separationVector * 2;
		}
		else
		{
			sphere1->m_position -= separationVector;
			sphere2->m_position += separationVector;
		}
		
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
		sphere->applyForce(2 * forceVector);
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
		box->applyForce(2 * forceVector);
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

	if (collision)
	{
		// determine direction of overlap
		float xOverlap = glm::min(b1max.x - b2min.x, b2max.x - b1min.x);
		float yOverlap = glm::min(b1max.y - b2min.y, b2max.y - b1min.y);
		float zOverlap = glm::min(b1max.z - b2min.z, b2max.z - b1min.z);

		float smallestOverlap = glm::min(glm::min(xOverlap, yOverlap), zOverlap);
		glm::vec3 collisionNormal;

		float delta = 0;
		int axis;

		// depending on which overlap is smallest, determine normal of collision
		// depending on which box is on which side, determine direction of separation
		if (smallestOverlap == xOverlap)
		{
			axis = 0;
			collisionNormal = glm::vec3(1, 0, 0);
			if (box1->GetMin().x < box2->GetMin().x)
			{
				delta = -1 * smallestOverlap / 2;
			}
			else
			{
				collisionNormal *= -1.0f;
				delta = smallestOverlap / 2;
			}
		}
		else if (smallestOverlap == yOverlap)
		{
			axis = 1;
			collisionNormal = glm::vec3(0, 1, 0);
			if (box1->GetMin().y < box2->GetMin().y)
			{
				delta = -1 * smallestOverlap / 2;
			}
			else
			{
				collisionNormal *= -1.0f;
				delta = smallestOverlap / 2;
			}
		}
		else if (smallestOverlap == zOverlap)
		{
			axis = 2;
			collisionNormal = glm::vec3(0, 0, 1);
			if (box1->GetMin().z < box2->GetMin().z)
			{
				delta = -1 * smallestOverlap / 2;
			}
			else
			{
				collisionNormal *= -1.0f;
				delta = smallestOverlap / 2;
			}
		}

		if (box1->m_static)
		{
			box2->m_position[axis] -= delta * 2;
		}
		else if (box2->m_static)
		{
			box1->m_position[axis] += delta * 2;
		}
		else
		{
			box1->m_position[axis] += delta;
			box2->m_position[axis] -= delta;
		}

		CollisionResponse(collisionNormal, box1, box2);

		//glm::vec3 relativeVelocity = box1->m_velocity - box2->m_velocity;
		//glm::vec3 collisionVector = collisionNormal * glm::abs(glm::dot(relativeVelocity, collisionNormal));
		//glm::vec3 forceVector = collisionVector * 1.0f / (1 / box1->m_mass + 1 / box2->m_mass);
		//// use newton's 3rd law to apply collision forces to bodies
		//box1->applyForceToActor(box2, -2 * forceVector);
		
	}

	return collision;
}

bool PhysicsScene::SphereAABBCollision(SphereClass* sphere, AABBClass* box)
{
	bool collision = false;
	float dist_squared = Squared(sphere->m_radius);

	glm::vec3 min = box->GetMin();
	glm::vec3 max = box->GetMax();
	glm::vec3 center = sphere->m_position;

	float xOverlap = 0; float yOverlap = 0; float zOverlap = 0;

	if (center.x < min.x)
	{
		xOverlap = Squared(center.x - min.x);
	}
	else if (center.x > max.x)
	{
		xOverlap = Squared(center.x - max.x);
	}

	if (center.y < min.y)
	{
		yOverlap = Squared(center.y - min.y);
	}
	else if (center.y > max.y)
	{
		yOverlap = Squared(center.y - max.y);
	}

	if (center.z < min.z)
	{
		zOverlap = Squared(center.z - min.z);
	}
	else if (center.z > max.z)
	{
		zOverlap = Squared(center.z - max.z);
	}

	dist_squared -= xOverlap;
	dist_squared -= yOverlap;
	dist_squared -= zOverlap;

	collision = dist_squared > 0;

	if (collision)
	{
		glm::vec3 b1min = box->GetMin();
		glm::vec3 b1max = box->GetMax();
		glm::vec3 b2min = sphere->m_position - glm::vec3(sphere->m_radius);
		glm::vec3 b2max = sphere->m_position + glm::vec3(sphere->m_radius);

		xOverlap = glm::min(b1max.x - b2min.x, b2max.x - b1min.x);
		yOverlap = glm::min(b1max.y - b2min.y, b2max.y - b1min.y);
		zOverlap = glm::min(b1max.z - b2min.z, b2max.z - b1min.z);

		float smallestOverlap = glm::min(glm::min(xOverlap, yOverlap), zOverlap);
		glm::vec3 collisionNormal;

		// depending on which overlap is smallest, determine normal of collision
		// depending on which box is on which side, determine direction of separation
		//if (smallestOverlap == xOverlap)
		//{
		//	collisionNormal = glm::vec3(1, 0, 0);
		//	if (box->GetCenter().x < sphere->m_position.x)
		//	{
		//		box->m_position.x -= smallestOverlap / 2;
		//		sphere->m_position.x += smallestOverlap / 2;
		//	}
		//	else
		//	{
		//		collisionNormal *= -1.0f;
		//		box->m_position.x += smallestOverlap / 2;
		//		sphere->m_position.x -= smallestOverlap / 2;
		//	}
		//}
		//else if (smallestOverlap == yOverlap)
		//{
		//	collisionNormal = glm::vec3(0, 1, 0);
		//	if (box->GetCenter().y < sphere->m_position.y)
		//	{
		//		box->m_position.y -= smallestOverlap / 2;
		//		sphere->m_position.y += smallestOverlap / 2;
		//	}
		//	else
		//	{
		//		collisionNormal *= -1.0f;
		//		box->m_position.y += smallestOverlap / 2;
		//		sphere->m_position.y -= smallestOverlap / 2;
		//	}
		//}
		//else if (smallestOverlap == zOverlap)
		//{
		//	collisionNormal = glm::vec3(0, 0, 1);
		//	if (box->GetCenter().z < sphere->m_position.z)
		//	{
		//		box->m_position.z -= smallestOverlap / 2;
		//		sphere->m_position.z += smallestOverlap / 2;
		//	}
		//	else
		//	{
		//		collisionNormal *= -1.0f;
		//		box->m_position.z += smallestOverlap / 2;
		//		sphere->m_position.z -= smallestOverlap / 2;
		//	}
		//}

		float delta = 0;
		int axis;

		if (smallestOverlap == xOverlap)
		{
			axis = 0;
			collisionNormal = glm::vec3(1, 0, 0);
			if (box->GetCenter().x < sphere->m_position.x)
			{
				delta = -1 * smallestOverlap / 2;
			}
			else
			{
				collisionNormal *= -1.0f;
				delta = smallestOverlap / 2;
			}
		}
		else if (smallestOverlap == yOverlap)
		{
			axis = 1;
			collisionNormal = glm::vec3(0, 1, 0);
			if (box->GetCenter().y < sphere->m_position.y)
			{
				delta = -1 * smallestOverlap / 2;
			}
			else
			{
				collisionNormal *= -1.0f;
				delta = smallestOverlap / 2;
			}
		}
		else if (smallestOverlap == zOverlap)
		{
			axis = 2;
			collisionNormal = glm::vec3(0, 0, 1);
			if (box->GetCenter().z < sphere->m_position.z)
			{
				delta = -1 * smallestOverlap / 2;
			}
			else
			{
				collisionNormal *= -1.0f;
				delta = smallestOverlap / 2;
			}
		}

		if (box->m_static)
		{
			sphere->m_position[axis] -= delta * 2;
		}
		else if (sphere->m_static)
		{
			box->m_position[axis] += delta * 2;
		}
		else
		{
			box->m_position[axis] += delta;
			sphere->m_position[axis] -= delta;
		}

		CollisionResponse(collisionNormal, box, sphere);
	}

	return collision;

}
