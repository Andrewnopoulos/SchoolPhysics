#pragma once

#include <vector>
#include "PhysicsObject.h"

class PhysicsScene
{
public:
	glm::vec2 m_gravity;
	float m_timestep;
	float AR = 16.0f / 9.0f; // aspect ratio
	std::vector<PhysicsObject*> m_actors;
	void AddActor(PhysicsObject*);
	void RemoveActor(PhysicsObject*);
	void Update();
	void DebugScene();
	void AddGizmos();
	void UpdateGizmos();
	void UpdateGizmos(glm::mat4 cameraTransform);
	PhysicsScene();
	~PhysicsScene();
};

