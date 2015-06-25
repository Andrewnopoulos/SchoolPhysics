#include "PhysicsTute.h"
#include "SphereClass.h"
#include "PhysicsScene.h"
#include "Plane.h"

#include <iostream>

void PhysicsTute::Startup()
{
	physicsScene = new PhysicsScene();
	physicsScene->m_gravity = glm::vec2(0, -1000.0f);
	physicsScene->m_timestep = 0.001f;
	physicsScene->m_ground = new Plane(glm::vec2(0, 1), 1.0f);
	// add balls to sim
	SphereClass *newBall;

	newBall = new SphereClass(glm::vec2(7, 10), glm::vec2(0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall);
	newBall = new SphereClass(glm::vec2(10, 10), glm::vec2(0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall);

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
		glm::vec2 separation = physicsScene->m_actors[1]->m_position - physicsScene->m_actors[0]->m_position;
		physicsScene->m_actors[0]->applyForceToActor(physicsScene->m_actors[1], separation * 100 / glm::length(separation));
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		physicsScene->m_actors[0]->applyForce(glm::vec2(0, 10.0f));
		physicsScene->m_actors[1]->applyForce(glm::vec2(0, 20.0f));
	}

	if (physicsScene->SphereSphereCollision((SphereClass*)physicsScene->m_actors[0], (SphereClass*)physicsScene->m_actors[1]))
	{
		std::cout << "True" << std::endl;
	}

	if (physicsScene->SpherePlaneCollision((SphereClass*)physicsScene->m_actors[0], physicsScene->m_ground))
	{
		std::cout << "Bounce" << std::endl;
	}

	if (physicsScene->SpherePlaneCollision((SphereClass*)physicsScene->m_actors[1], physicsScene->m_ground))
	{
		std::cout << "Bounce" << std::endl;
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