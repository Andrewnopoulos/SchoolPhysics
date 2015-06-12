#pragma once

#include <glm\glm.hpp>
#include <vector>
#include "PhysicsObject.h"

class PhysicsScene
{
public:
	glm::vec2 m_gravity;
	float m_timestep;
	std::vector<PhysicsObject*> m_actors;
	void AddActor(PhysicsObject*);
	void RemoveActor(PhysicsObject*);
	void Update();
	void DebugScene();
	void addGizmos();
	PhysicsScene();
	~PhysicsScene();
};

