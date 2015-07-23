#include "AABBClass.h"


AABBClass::AABBClass(glm::vec3 position, glm::vec3 velocity, float mass, float a_width, float a_height, float a_depth) 
: RigidBody(position, velocity, 0, mass)
{
	width = a_width;
	height = a_height;
	depth = a_depth;
}

AABBClass::~AABBClass()
{
}

