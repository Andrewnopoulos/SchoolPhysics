#pragma once
#include "Application.h"
#include "FlyCamera.h"

class PhysicsScene;

class PhysicsTute : public Application 
{
public:
	void Startup();
	void Destroy();
	void Update();
	void Draw();

private:
	PhysicsScene* physicsScene;
	float previousTime;
	float currentTime;
	FlyCamera myCam;
};

