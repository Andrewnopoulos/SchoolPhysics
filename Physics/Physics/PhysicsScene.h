#pragma once

#include <vector>
#include "PhysicsObject.h"

class Plane;
class RigidBody;
class SphereClass;
class AABBClass;

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
	bool AABBAABBCollision(AABBClass* box1, AABBClass* box2);
	bool SphereAABBCollision(SphereClass* sphere, AABBClass* box);
	bool AABBPlaneCollision(AABBClass* box, Plane* plane);
	bool RigidPlaneCollision(RigidBody* object, Plane* plane);

	inline float Squared(float input) { return input * input; }

	PhysicsScene();
	~PhysicsScene();
};

