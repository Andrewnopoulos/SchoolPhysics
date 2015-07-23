#pragma once
#include "RigidBody.h"
class AABBClass :
	public RigidBody
{
private:
	float width, height, depth;

public:
	AABBClass(glm::vec3 position, glm::vec3 velocity, float mass, float a_width, float a_height, float a_depth);
	~AABBClass();
};

