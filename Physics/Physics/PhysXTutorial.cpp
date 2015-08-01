#include "PhysXTutorial.h"
#include <Gizmos.h>

void PhysXTutorial::Startup()
{
	SetUpPhysX();
	SetUpVisualDebugger();
	SetUpEnvironment();

	Gizmos::create();

	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(50);
	myCam.setRotationSpeed(2.0f);

	muzzlespeed = -70.0f;

	cooldown = 2.0f;

}

void PhysXTutorial::FireBall()
{
	glm::mat4 t = myCam.getWorldTransform();
	glm::vec3 startingPosition(t[3]);
	glm::vec3 direction(t[2]);
	physx::PxVec3 velocity = physx::PxVec3(direction.x, direction.y, direction.z) * muzzlespeed;

	//add a box
	float density = 10;
	PxSphereGeometry sphere(2);
	PxTransform transform(PxVec3(startingPosition.x, startingPosition.y, startingPosition.z));
	PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, sphere, *g_PhysicsMaterial, density);
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
	g_PhysXActors.push_back(dynamicActor);

	dynamicActor->setLinearVelocity(velocity, true);
}

void PhysXTutorial::Update()
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	myCam.update(deltaTime);

	cooldown -= deltaTime;

	if (deltaTime <= 0)
	{
		return;
	}
	g_PhysicsScene->simulate(deltaTime);
	while (g_PhysicsScene->fetchResults() == false)
	{
		// don’t need to do anything here yet but we have to fetch results
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
	{
		if (cooldown < 0)
		{
			FireBall();
			cooldown = 2.0f;
		}
	}

	//// box adding code
	//float density = 10;
	//PxBoxGeometry box(2, 2, 2);
	//PxTransform transform(PxVec3(0, 5, 0));
	//PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box,
	//	*g_PhysicsMaterial, density);
	////add it to the physX scene
	//g_PhysicsScene->addActor(*dynamicActor);
	//g_PhysXActors.push_back(dynamicActor);

}

void PhysXTutorial::addWidget(PxShape* shape, PxRigidActor* actor)
{
	PxGeometryType::Enum type = shape->getGeometryType();
	switch (type)
	{
	case PxGeometryType::eBOX:
		addBox(shape, actor);
		break;
	case PxGeometryType::eSPHERE:
		addSphere(shape, actor);
		break;
	}
}

void PhysXTutorial::addBox(PxShape* pShape, PxRigidActor* actor)
{
	//get the geometry for this PhysX collision volume
	PxBoxGeometry geometry;
	float width = 1, height = 1, length = 1;
	bool status = pShape->getBoxGeometry(geometry);
	if (status)
	{
		width = geometry.halfExtents.x;
		height = geometry.halfExtents.y;
		length = geometry.halfExtents.z;
	}
	//get the transform for this PhysX collision volume
	PxMat44 m(PxShapeExt::getGlobalPose(*pShape, *actor));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
		m.column1.x, m.column1.y, m.column1.z, m.column1.w,
		m.column2.x, m.column2.y, m.column2.z, m.column2.w,
		m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	//get the position out of the transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec3 extents = glm::vec3(width, height, length);
	glm::vec4 colour = glm::vec4(1, 0, 0, 1);
	if (actor->getName() != NULL && strcmp(actor->getName(), "Pickup1")) //seriously horrid hack so I can show pickups a different colour
	{
		colour = glm::vec4(0, 1, 0, 1);
	}
		//create our box gizmo
	Gizmos::addAABBFilled(position, extents, colour, &M);
}

void PhysXTutorial::addSphere(PxShape* pShape, PxRigidActor* actor)
{
	PxSphereGeometry geometry;
	float radius = 1;
	//get the geometry for this PhysX collision volume
	bool status = pShape->getSphereGeometry(geometry);
	if (status)
	{
		radius = geometry.radius;
	}
	//get the transform for this PhysX collision volume
	PxMat44 m(PxShapeExt::getGlobalPose(*pShape, *actor));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
		m.column1.x, m.column1.y, m.column1.z, m.column1.w,
		m.column2.x, m.column2.y, m.column2.z, m.column2.w,
		m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	//get the position out of the transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec4 colour = glm::vec4(1, 0, 0, 1);
	if (actor->getName() != NULL && strcmp(actor->getName(), "Pickup1")) //seriously horrid hack so I can show pickups a different colour
	{
		colour = glm::vec4(0, 1, 0, 1);
	}
	Gizmos::addSphere(position, radius, 7, 7, colour, &M);
}

void PhysXTutorial::Draw()
{
	Gizmos::clear();
	// Add widgets to represent all the phsyX actors which are in the scene
	for (auto actor : g_PhysXActors)
	{
		{
			PxU32 nShapes = actor->getNbShapes();
			PxShape** shapes = new PxShape*[nShapes];
			actor->getShapes(shapes, nShapes);
			// Render all the shapes in the physx actor (for early tutorials there is just one)
			while (nShapes--)
			{
				addWidget(shapes[nShapes], actor);
			}
			delete[] shapes;
		}
	}
	Gizmos::draw(myCam.getProjectionView());
}
void PhysXTutorial::Destroy()
{
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();

	Gizmos::destroy();
}

void PhysXTutorial::SetUpPhysX()
{
	PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback,
		gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation,
		PxTolerancesScale());
	PxInitExtensions(*g_Physics);
	//create physics material 
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -10.0f, 0);
	sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);
}

void PhysXTutorial::SetUpVisualDebugger()
{
	// check if PvdConnection manager is available on this platform
	if (g_Physics->getPvdConnectionManager() == NULL)
		return;
	// setup connection parameters
	const char* pvd_host_ip = "127.0.0.1";
	// IP of the PC which is running PVD
	int port = 5425;
	// TCP port to connect to, where PVD is listening
	unsigned int timeout = 100;
	// timeout in milliseconds to wait for PVD to respond,
	//consoles and remote PCs need a higher timeout.
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	// and now try to connectPxVisualDebuggerExt
	auto theConnection = PxVisualDebuggerExt::createConnection(
		g_Physics->getPvdConnectionManager(), pvd_host_ip, port, timeout, connectionFlags);
}

void PhysXTutorial::SetUpEnvironment()
{
	//add a plane
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi*1.0f,
		PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(),
		*g_PhysicsMaterial);
	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);
	//add a box
	float density = 10;
	PxBoxGeometry box(2, 2, 2);
	PxTransform transform(PxVec3(0, 5, 0));
	PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box,
		*g_PhysicsMaterial, density);
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
	g_PhysXActors.push_back(dynamicActor);

}