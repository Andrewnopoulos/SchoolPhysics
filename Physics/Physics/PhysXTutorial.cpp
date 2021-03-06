#include "PhysXTutorial.h"
#include <Gizmos.h>
#include "ParticleEmitter.h"
#include "ParticleFluidEmitter.h"
#include <iostream>

//complex humanoid ragdoll example
RagdollNode* ragdollData[] =
{
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), NO_PARENT, 1, 3, 1, 1, "lower spine"),
	new RagdollNode(PxQuat(PxPi, Z_AXIS), LOWER_SPINE, 1, 1, -1, 1, "left pelvis"),
	new RagdollNode(PxQuat(0, Z_AXIS), LOWER_SPINE, 1, 1, -1, 1, "right pelvis"),
	new RagdollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS), LEFT_PELVIS, 5, 2, -1, 1, "L upper leg"),
	new RagdollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS), RIGHT_PELVIS, 5, 2, -1, 1, "R upper leg"),
	new RagdollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS), LEFT_UPPER_LEG, 5, 1.75, -1, 1, "L lower leg"),
	new RagdollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS), RIGHT_UPPER_LEG, 5, 1.75, -1, 1, "R lowerleg"),
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), LOWER_SPINE, 1, 3, 1, -1, "upper spine"),
	new RagdollNode(PxQuat(PxPi, Z_AXIS), UPPER_SPINE, 1, 1.5, 1, 1, "left clavicle"),
	new RagdollNode(PxQuat(0, Z_AXIS), UPPER_SPINE, 1, 1.5, 1, 1, "right clavicle"),
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), UPPER_SPINE, 1, 1, 1, -1, "neck"),
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), NECK, 1, 3, 1, -1, "HEAD"),
	new RagdollNode(PxQuat(PxPi - .3, Z_AXIS), LEFT_CLAVICLE, 3, 1.5, -1, 1, "left upper arm"),
	new RagdollNode(PxQuat(0.3, Z_AXIS), RIGHT_CLAVICLE, 3, 1.5, -1, 1, "right upper arm"),
	new RagdollNode(PxQuat(PxPi - .3, Z_AXIS), LEFT_UPPER_ARM, 3, 1, -1, 1, "left lower arm"),
	new RagdollNode(PxQuat(0.3, Z_AXIS), RIGHT_UPPER_ARM, 3, 1, -1, 1, "right lower arm"),
	NULL
};

void PhysXTutorial::Startup()
{
	SetUpPhysX();
	SetUpVisualDebugger();
	SetUpEnvironment();
	FluidInit();
	SetUpPlayer();

	PxArticulation* ragDollArticulation;
	ragDollArticulation = makeRagdoll(g_Physics, ragdollData, PxTransform(PxVec3(0, 0, 0)), 0.1f, g_PhysicsMaterial);
	g_PhysicsScene->addArticulation(*ragDollArticulation);

	Gizmos::create();

	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(50);
	myCam.setRotationSpeed(0.01f);

	muzzlespeed = -60.0f;

	cooldown = 0.5f;

	deltaAvg = 0;
	samples = 0;

}

void PhysXTutorial::SetUpPlayer()
{
	myHitReport = new MyControllerHitReport();
	gCharacterManager = PxCreateControllerManager(*g_PhysicsScene);
	
	//describe our controller...
	PxCapsuleControllerDesc desc;
	desc.height = 1.6f;
	desc.radius = 0.4f;
	desc.position.set(0, 0, 0);
	playerPhysicsMaterial = g_Physics->createMaterial(0.5f, 0.5f, 0.5f);
	desc.material = playerPhysicsMaterial;
	desc.reportCallback = myHitReport; //connect it to our collision detection routine
	desc.density = 10;
	//create the layer controller
	gPlayerController = gCharacterManager->createController(desc);

	gPlayerController->setPosition(PxExtendedVec3(0, 5, 0));
	//set up some variables to control our player with
	_characterYVelocity = 0; //initialize character velocity
	_characterRotation = 0; //and rotation
	_playerGravity = -0.5f; //set up the player gravity
	myHitReport->clearPlayerContactNormal(); //initialize the contact normal (what we are in contact with)
	g_PhysicsScene->addActor(*gPlayerController->getActor());
	g_PhysXActors.push_back(gPlayerController->getActor());

	setupFiltering(gPlayerController->getActor(), FilterGroup::ePLAYER, FilterGroup::ePLATFORM);

}

void PhysXTutorial::FluidInit()
{
	PxParticleFluid* pf;
	// create particle system in PhysX SDK
	// set immutable properties.
	PxU32 maxParticles = 1000;
	bool perParticleRestOffset = false;
	pf = g_Physics->createParticleFluid(maxParticles, perParticleRestOffset);

	pf->setRestParticleDistance(0.3f);
	pf->setDynamicFriction(0.1);
	pf->setStaticFriction(0.1);
	pf->setDamping(0.1);
	pf->setParticleMass(.1);
	pf->setRestitution(0);
	//pf->setParticleReadDataFlag(PxParticleReadDataFlag::eDENSITY_BUFFER,
	// true);
	pf->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY, true);
	pf->setStiffness(100);
	if (pf)
	{
		g_PhysicsScene->addActor(*pf);
		m_particleEmitter = new ParticleFluidEmitter(maxParticles,
			PxVec3(0, 10, 0), pf, .1);
		m_particleEmitter->setStartVelocityRange(-0.001f, -250.0f, -0.001f,
			0.001f, -250.0f, 0.001f);
	}
}

PxArticulation* PhysXTutorial::makeRagdoll(PxPhysics* g_Physics, RagdollNode** nodeArray,
	PxTransform worldPos, float scaleFactor, PxMaterial* ragdollMaterial)
{
	//create the articulation for our ragdoll
	PxArticulation *articulation = g_Physics->createArticulation();
	RagdollNode** currentNode = nodeArray;
	//while there are more nodes to process...
	while (*currentNode != NULL)
	{
		//get a pointer to the current node
		RagdollNode* currentNodePtr = *currentNode;
		//create a pointer ready to hold the parent node pointer if there is one
		RagdollNode* parentNode = nullptr;
		//get scaled values for capsule
		float radius = currentNodePtr->radius*scaleFactor;
		float halfLength = currentNodePtr->halfLength*scaleFactor;
		float childHalfLength = radius + halfLength;
		float parentHalfLength = 0; //will be set later if there is a parent
		//get a pointer to the parent
		PxArticulationLink* parentLinkPtr = NULL;
		currentNodePtr->scaledGobalPos = worldPos.p;

		if (currentNodePtr->parentNodeIdx != -1)
		{
			//if there is a parent then we need to work out our local position for the link
			//get a pointer to the parent node
			parentNode = *(nodeArray + currentNodePtr->parentNodeIdx);
			//get a pointer to the link for the parent
			parentLinkPtr = parentNode->linkPtr;
			parentHalfLength = (parentNode->radius + parentNode->halfLength) *scaleFactor;
			//work out the local position of the node
			PxVec3 currentRelative = currentNodePtr->childLinkPos * currentNodePtr ->globalRotation.rotate(PxVec3(childHalfLength, 0, 0));
			PxVec3 parentRelative = -currentNodePtr->parentLinkPos * parentNode ->globalRotation.rotate(PxVec3(parentHalfLength, 0, 0));
			currentNodePtr->scaledGobalPos = parentNode->scaledGobalPos - (parentRelative +
				currentRelative);
		}

		//build the transform for the link
		PxTransform linkTransform = PxTransform(currentNodePtr->scaledGobalPos, currentNodePtr ->globalRotation);
		//create the link in the articulation
		PxArticulationLink* link = articulation->createLink(parentLinkPtr, linkTransform);
		//add the pointer to this link into the ragdoll data so we have it for later when we want to link to it
		currentNodePtr->linkPtr = link;
		float jointSpace = .01f; //gap between joints
		float capsuleHalfLength = (halfLength > jointSpace ? halfLength - jointSpace : 0) + .01f;
		PxCapsuleGeometry capsule(radius, capsuleHalfLength);
		link->createShape(capsule, *ragdollMaterial); //adds a capsule collider to the link
		PxRigidBodyExt::updateMassAndInertia(*link, 50.0f); //adds some mass, mass should really be part of the data!

		if (currentNodePtr->parentNodeIdx != -1)
		{
			//get the pointer to the joint from the link
			PxArticulationJoint *joint = link->getInboundJoint();
			//get the relative rotation of this link
			PxQuat frameRotation = parentNode->globalRotation.getConjugate() *
				currentNodePtr->globalRotation;
			//set the parent contraint frame
			PxTransform parentConstraintFrame = PxTransform(PxVec3(currentNodePtr ->parentLinkPos * parentHalfLength, 0, 0), frameRotation);
			//set the child constraint frame (this the constraint frame of the newly added link)
			PxTransform thisConstraintFrame = PxTransform(PxVec3(currentNodePtr ->childLinkPos * childHalfLength, 0, 0));
			//set up the poses for the joint so it is in the correct place
			joint->setParentPose(parentConstraintFrame);
			joint->setChildPose(thisConstraintFrame);
			//set up some constraints to stop it flopping around
			joint->setStiffness(20);
			joint->setDamping(20);
			joint->setSwingLimit(0.4f, 0.4f);
			joint->setSwingLimitEnabled(true);
			joint->setTwistLimit(-0.1f, 0.1f);
			joint->setTwistLimitEnabled(true);
		}
		currentNode++;
	}
	g_PhysXActorsRagDolls.push_back(articulation);
	return articulation;
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

void PhysXTutorial::PlayerInput(float a_deltaTime)
{
	bool onGround; //set to true if we are on the ground
	float movementSpeed = 10.0f; //forward and back movement speed
	float rotationSpeed = 1.0f; //turn speed
	//check if we have a contact normal. if y is greater than .3 we assume this is solid ground.This is a rather primitive way to do this.Can you do better ?
	if (myHitReport->getPlayerContactNormal().y > 0.3f)
	{
		_characterYVelocity = -0.1f;
		onGround = true;
	}
	else
	{
		_characterYVelocity += _playerGravity * a_deltaTime;
		onGround = false;
	}
	myHitReport->clearPlayerContactNormal();
	const PxVec3 up(0, 1, 0);
	//scan the keys and set up our intended velocity based on a global transform
	PxVec3 velocity(0, _characterYVelocity, 0);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		velocity.x -= movementSpeed*a_deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		velocity.x += movementSpeed*a_deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		velocity.z += movementSpeed * a_deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		velocity.z -= movementSpeed * a_deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		velocity.y += movementSpeed * a_deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		_characterRotation -= rotationSpeed * a_deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		_characterRotation += rotationSpeed * a_deltaTime;
	}

	//To do.. add code to control z movement and jumping
	float minDistance = 0.001f;
	PxControllerFilters filter;
	//make controls relative to player facing
	PxQuat rotation(_characterRotation, PxVec3(0, 1, 0));
	//move the controller
	gPlayerController->move(rotation.rotate(velocity), minDistance, a_deltaTime,
		filter);

	PxExtendedVec3 pos = gPlayerController->getPosition();
	myCam.setPosition(glm::vec3(pos.x, pos.y + 5, pos.z));
}

void PhysXTutorial::Update()
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	myCam.update(deltaTime);

	time += deltaTime;
	cooldown -= deltaTime;

	if (deltaTime <= 0)
	{
		return;
	}
	g_PhysicsScene->simulate(deltaTime);
	while (g_PhysicsScene->fetchResults() == false)
	{
		// don�t need to do anything here yet but we have to fetch results
	}

	deltaAvg += deltaTime;
	samples += 1;

	if (time >= 2)
	{
		deltaAvg /= samples;
		time = 0;
		float frameRate = 1.0f / deltaAvg;
		char buffer[50];
		sprintf(buffer, "The frame rate is %f", frameRate);
		glfwSetWindowTitle(window, buffer);
		deltaAvg = 0;
		samples = 0;
	}

	PlayerInput(deltaTime);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
	{
		if (cooldown < 0)
		{
			FireBall();
			cooldown = 0.5f;
		}
	}

	if (m_particleEmitter)
	{
		m_particleEmitter->update(deltaTime);
		//render all our particles
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
	case PxGeometryType::eCAPSULE:
		addCapsule(shape, actor);
		break;
	default:
		addSphere(shape, actor);
		break;
	}
}

void PhysXTutorial::addCapsule(PxShape* pShape, PxRigidActor* actor)
{
	PxCapsuleGeometry capsuleGeometry;
	float radius = 1.0f;
	float halfHeight = 1.0f;
	bool status = pShape->getCapsuleGeometry(capsuleGeometry);
	if (status)
	{
		radius = capsuleGeometry.radius; //copy out capsule radius
		halfHeight = capsuleGeometry.halfHeight; //copy out capsule half length
	}
	PxTransform transform = PxShapeExt::getGlobalPose(*pShape, *actor);

	PxMat44 m(transform); //Create a rotation matrix from transform
	glm::mat4* M = (glm::mat4*)(&m);
	//get the world position from the PhysX tranform
	glm::vec3 position;
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;

	glm::vec4 axis(halfHeight, 0, 0, 0); //axis for the capsule
	axis = *M * axis; //rotate axis to correct orientation
	//add our 2 end cap spheres...
	glm::vec4 colour = glm::vec4(1, 0, 0, 1);
	Gizmos::addSphere(position + axis.xyz(), radius, 10, 10, colour);
	Gizmos::addSphere(position - axis.xyz(), radius, 10, 10, colour);
	//Fix the gizmo rotation
	glm::mat4 m2 = glm::rotate(*M, 11 / 7.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	Gizmos::addCylinderFilled(position, radius, halfHeight, 10, colour, &m2);

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
	float radius = 0.5f;
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
	Gizmos::addSphere(position, radius, 6, 6, colour, &M);
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

	for (auto articulation : g_PhysXActorsRagDolls)
	{
		PxU32 nLinks = articulation->getNbLinks();
		PxArticulationLink** links = new PxArticulationLink*[nLinks];
		articulation->getLinks(links, nLinks);
		while (nLinks--)
		{
			PxArticulationLink* link = links[nLinks];
			PxU32 nShapes = link->getNbShapes();
			PxShape** shapes = new PxShape*[nShapes];
			link->getShapes(shapes, nShapes);
			while (nShapes--)
			{
				addWidget(shapes[nShapes], link);
			}
		}
		delete[] links;
	}

	if (m_particleEmitter)
	{
		m_particleEmitter->renderParticles();
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

PxFilterFlags MyFilterShader(PxFilterObjectAttributes attributes0, PxFilterData
	filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) ||
		PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 &
		filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND |
		PxPairFlag::eNOTIFY_TOUCH_LOST;
	return PxFilterFlag::eDEFAULT;
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
	sceneDesc.filterShader = MyFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);

	PxSimulationEventCallback* mycollisionCallBack = this;
	g_PhysicsScene->setSimulationEventCallback(mycollisionCallBack);
}

//helper function to set up filtering
void PhysXTutorial::setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32
	filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask; // word1 = ID mask to filter pairs that trigger a contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*)*numShapes, 16);

	actor->getShapes(shapes, numShapes);

	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}

	_aligned_free(shapes);
}

void PhysXTutorial::setShapeAsTrigger(PxRigidActor* actorIn)
{
	PxRigidStatic* staticActor = actorIn->is<PxRigidStatic>();
	assert(staticActor);
	const PxU32 numShapes = staticActor->getNbShapes();
	PxShape** shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*)*numShapes, 16);
	staticActor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		shapes[i]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shapes[i]->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
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
	//PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi*1.0f,
	//	PxVec3(0.0f, 0.0f, 1.0f)));
	//PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(),
	//	*g_PhysicsMaterial);
	////add it to the physX scene
	//g_PhysicsScene->addActor(*plane);

	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(), *g_PhysicsMaterial);

	const PxU32 numShapes = plane->getNbShapes();
	g_PhysicsScene->addActor(*plane);

	PxBoxGeometry side1(4.5, 1, .5);
	PxBoxGeometry side2(.5, 1, 4.5);
	pose = PxTransform(PxVec3(0.0f, 0.5, 4.0f));
	PxRigidStatic* box = PxCreateStatic(*g_Physics, pose, side1, *g_PhysicsMaterial);

	g_PhysicsScene->addActor(*box);
	g_PhysXActors.push_back(box);

	pose = PxTransform(PxVec3(0.0f, 0.5, -4.0f));
	box = PxCreateStatic(*g_Physics, pose, side1, *g_PhysicsMaterial);
	g_PhysicsScene->addActor(*box);
	g_PhysXActors.push_back(box);

	pose = PxTransform(PxVec3(4.0f, 0.5, 0));
	box = PxCreateStatic(*g_Physics, pose, side2, *g_PhysicsMaterial);
	g_PhysicsScene->addActor(*box);
	g_PhysXActors.push_back(box);

	pose = PxTransform(PxVec3(-4.0f, 0.5, 0));
	box = PxCreateStatic(*g_Physics, pose, side2, *g_PhysicsMaterial);
	g_PhysicsScene->addActor(*box);
	g_PhysXActors.push_back(box);

	pose = PxTransform(PxVec3(-4.0f, 0.5, 10));
	box = PxCreateStatic(*g_Physics, pose, side2, *g_PhysicsMaterial);
	setShapeAsTrigger(box);
	setupFiltering(box, FilterGroup::ePLATFORM, FilterGroup::ePLAYER);
	g_PhysicsScene->addActor(*box);
	g_PhysXActors.push_back(box);

	//PxCapsuleGeometry capsule(5, 7);
	//PxTransform transform(PxVec3(0, 5, 0));
	//float density = 10;
	//PxRigidDynamic* pXactor = PxCreateDynamic(*g_Physics, transform, capsule, *g_PhysicsMaterial, density);
	//g_PhysicsScene->addActor(*pXactor);
	//g_PhysXActors.push_back(pXactor);

	//add a box
	//float density = 10;
	//PxBoxGeometry box(2, 2, 2);
	//PxTransform transform(PxVec3(0, 5, 0));
	//PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box,
	//	*g_PhysicsMaterial, density);
	////add it to the physX scene
	//g_PhysicsScene->addActor(*dynamicActor);
	//g_PhysXActors.push_back(dynamicActor);

}

void PhysXTutorial::SpawnBox()
{
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

void MyControllerHitReport::onShapeHit(const PxControllerShapeHit &hit)
{
	//gets a reference to a structure which tells us what has been hit and where
	//get the acter from the shape we hit
	PxRigidActor* actor = hit.shape->getActor();
	//get the normal of the thing we hit and store it so the player controller can respond correctly
	_playerContactNormal = hit.worldNormal;
	//try to cast to a dynamic actor
	PxRigidDynamic* myActor = actor->is<PxRigidDynamic>();
	if (myActor)
	{
		//this is where we can apply forces to things we hit
	}
}

void PhysXTutorial::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs,
	PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];
		//only interested in touches found and lost
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			/*cout << "Collision Detected between: ";
			cout << pairHeader.actors[0]->getName();
			cout << pairHeader.actors[1]->getName() << endl;*/
		}
	}
}

void PhysXTutorial::onTrigger(PxTriggerPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		PxTriggerPair* pair = pairs + i;
		//PxActor* triggerActor = pair->triggerActor;
		//PxActor* otherActor = pair->otherActor;
		//std::cout << otherActor->getName();
		std::cout << " Inside Trigger " << std::endl;
		SpawnBox();
		//std::cout << triggerActor->getName() << std::endl;
	}
};
