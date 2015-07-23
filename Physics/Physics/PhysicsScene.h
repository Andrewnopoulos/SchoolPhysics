#pragma once

#include <vector>
#include "PhysicsObject.h"

class Plane;
class RigidBody;
class SphereClass;

class PhysicsScene
{
public:
	glm::vec3 m_gravity;
	float m_timestep;
	float AR = 16.0f / 9.0f; // aspect ratio
	std::vector<RigidBody*> m_actors;
	std::vector<Plane*> m_walls;
	void AddActor(RigidBody*);
	void RemoveActor(RigidBody*);
	void Update();
	void DebugScene();
	void AddGizmos();
	void UpdateGizmos();
	void UpdateGizmos(glm::mat4 cameraTransform);
	bool SphereSphereCollision(SphereClass* sphere1, SphereClass* sphere2);
	bool SpherePlaneCollision(SphereClass* sphere, Plane* plane);
	PhysicsScene();
	~PhysicsScene();
};

