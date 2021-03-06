#pragma once
#include "RigidBody.h"
class AABBClass :
	public RigidBody
{
private:
	glm::vec4 m_colour;

public:
	AABBClass(glm::vec3 position, glm::vec3 velocity, float mass, float elasticity, glm::vec3 a_dimensions, glm::vec4 a_colour);
	~AABBClass();

	glm::vec3 m_dimensions;

	void Debug() {};
	void MakeGizmo();

	inline glm::vec3 GetMin()
	{
		return m_position;
	}

	inline glm::vec3 GetMax()
	{
		return m_position + m_dimensions;
	}

	inline glm::vec3 GetCenter()
	{
		return m_position + (m_dimensions / 2.0f);
	}

	inline void ShiftBox(glm::vec3 a_movement)
	{
		m_position += a_movement;
	}

	void SetPosition(glm::vec3 a_position);
};

