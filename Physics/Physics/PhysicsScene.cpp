#include "PhysicsScene.h"
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

	Gizmos::draw2D(glm::ortho<float>(-100, 100, -100 / AR, 100 / AR, -1.0f, 1.0f));
}

void PhysicsScene::UpdateGizmos(glm::mat4 cameraTransform)
{
	Gizmos::clear();

	for each (PhysicsObject* var in m_actors)
	{
		var->MakeGizmo();
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