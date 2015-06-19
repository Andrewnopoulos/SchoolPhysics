#include "PhysicsTute.h"
#include "SphereClass.h"
#include "PhysicsScene.h"

void PhysicsTute::Startup()
{
	physicsScene = new PhysicsScene();
	physicsScene->m_gravity = glm::vec2(0, -10);
	physicsScene->m_timestep = 0.001f;
	// add balls to sim
	SphereClass *newBall;

	newBall = new SphereClass(glm::vec2(-40, 0), glm::vec2(0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall);
	newBall = new SphereClass(glm::vec2(-20, 0), glm::vec2(0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall);
	newBall = new SphereClass(glm::vec2(0, 0), glm::vec2(0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall);
	newBall = new SphereClass(glm::vec2(10, 0), glm::vec2(0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall);

	newBall = new SphereClass(glm::vec2(-40, 10), glm::vec2(0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall);
	newBall = new SphereClass(glm::vec2(-20, 10), glm::vec2(0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall);
	newBall = new SphereClass(glm::vec2(0, 10), glm::vec2(0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall);
	newBall = new SphereClass(glm::vec2(10, 10), glm::vec2(0, 0), 3.0f, 1, glm::vec4(1, 0, 0, 1));
	physicsScene->AddActor(newBall);

	currentTime = 0;
	previousTime = 0;

	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(5);
	myCam.setRotationSpeed(0.01f);
}

void PhysicsTute::Update()
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	
	myCam.update(deltaTime);

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