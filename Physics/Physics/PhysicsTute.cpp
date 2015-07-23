#include "PhysicsTute.h"
#include "SphereClass.h"
#include "PhysicsScene.h"
#include "AABBClass.h"
#include "Plane.h"

#include <iostream>

void PhysicsTute::Startup()
{
	physicsScene = new PhysicsScene();
	physicsScene->m_gravity = glm::vec3(0, -50000.0f, 0);
	physicsScene->m_timestep = 0.0001f;
	
	// add balls to sim
	SphereClass *newBall;

	newBall = new SphereClass(glm::vec3(7, 10, 0), glm::vec3(0, 0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall);
	newBall = new SphereClass(glm::vec3(10, 10, 0), glm::vec3(0, 0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall);

	Plane *newPlane;

	newPlane = new Plane(glm::vec3(0, 1, 0), 1.0f);
	physicsScene->m_walls.push_back(newPlane);

	newPlane = new Plane(glm::vec3(0, -1, 0), -50.0f);
	physicsScene->m_walls.push_back(newPlane);

	newPlane = new Plane(glm::vec3(-1, 0, 0), -50.0f);
	physicsScene->m_walls.push_back(newPlane);

	newPlane = new Plane(glm::vec3(1, 0, 0), -50.0f);
	physicsScene->m_walls.push_back(newPlane);

	currentTime = 0;
	previousTime = 0;

	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(50);
	myCam.setRotationSpeed(0.01f);
}

void PhysicsTute::Update()
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	
	myCam.update(deltaTime);

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		glm::vec3 separation = physicsScene->m_actors[1]->m_position - physicsScene->m_actors[0]->m_position;
		physicsScene->m_actors[0]->applyForceToActor(physicsScene->m_actors[1], separation * 300 / glm::length(separation));
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		physicsScene->m_actors[0]->applyForce(glm::vec3(0, 10.0f, 20.0f));
		physicsScene->m_actors[1]->applyForce(glm::vec3(0, 20.0f, -10.0f));
	}

	if (physicsScene->SphereSphereCollision((SphereClass*)physicsScene->m_actors[0], (SphereClass*)physicsScene->m_actors[1]))
	{
		std::cout << "True" << std::endl;
	}

	for each(RigidBody* var in physicsScene->m_actors)
	{
		for each(Plane* plane in physicsScene->m_walls)
		{
			physicsScene->SpherePlaneCollision((SphereClass*)var, plane);
		}
	}

	physicsScene->Update();
}

void PhysicsTute::Draw()
{
	physicsScene->UpdateGizmos(myCam.getProjectionView());
}

void PhysicsTute::Destroy()
{
	delete physicsScene;
}