#include "PhysicsScene.h"
#include "RigidBody.h"
#include "SphereClass.h"
#include "Plane.h"
#include "AABBClass.h"
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

void PhysicsScene::AddActor(RigidBody* a_object)
{
	m_actors.push_back(a_object);
}

void PhysicsScene::RemoveActor(RigidBody* a_object)
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
		sphere->applyForce(2 * forceVector);
		sphere->m_position += collisionNormal * intersection * 1.5f;
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