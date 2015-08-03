#pragma once

#include "Application.h"

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd\PxVisualDebugger.h>
#include <vector>
#include "FlyCamera.h"

using namespace physx;

class myAllocator : public PxAllocatorCallback
{
public:
	virtual ~myAllocator() {}
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
	{
		void* pointer = _aligned_malloc(size, 16);
		return pointer;
	}
	virtual void deallocate(void* ptr)
	{
		_aligned_free(ptr);
	}
};

//Parts which make up our ragdoll
enum RagDollParts
{
	NO_PARENT = -1,
	LOWER_SPINE,
	LEFT_PELVIS,
	RIGHT_PELVIS,
	LEFT_UPPER_LEG,
	RIGHT_UPPER_LEG,
	LEFT_LOWER_LEG,
	RIGHT_LOWER_LEG,
	UPPER_SPINE,
	LEFT_CLAVICLE,
	RIGHT_CLAVICLE,
	NECK,
	HEAD,
	LEFT_UPPER_ARM,
	RIGHT_UPPER_ARM,
	LEFT_LOWER_ARM,
	RIGHT_LOWER_ARM,
};

//create some constants for axis of rotation to make definition of quaternions a bit neater
const PxVec3 X_AXIS = PxVec3(1, 0, 0);
const PxVec3 Y_AXIS = PxVec3(0, 1, 0);
const PxVec3 Z_AXIS = PxVec3(0, 0, 1);struct RagdollNode
{
	PxQuat globalRotation; //rotation of this link in model space - we could have done this relative to the parent node but it's harder to visualize when setting up the data by hand
		PxVec3 scaledGobalPos; //Position of the link centre in world space which is calculated when we process the node.It's easiest if we store it here so we have it when we transform the child
		int parentNodeIdx; //Index of the parent node
	float halfLength; //half length of the capsule for this node
	float radius; //radius of capsule for thisndoe
	float parentLinkPos; //relative position of link centre in parent to this node. 0 is the centre of hte node, -1 is left end of capsule and 1 is right end of capsule relative to x
		float childLinkPos; //relative position of link centre in child
	char* name; //name of link
	PxArticulationLink* linkPtr;
	//constructor
	RagdollNode(PxQuat _globalRotation, int _parentNodeIdx, float _halfLength, float
		_radius, float _parentLinkPos, float _childLinkPos, char* _name){
		globalRotation =
			_globalRotation, parentNodeIdx = _parentNodeIdx; halfLength = _halfLength; radius = _radius;
		parentLinkPos = _parentLinkPos; childLinkPos = _childLinkPos; name = _name;
	};
};

class PhysXTutorial : public Application
{
	void Startup();
	void Destroy();
	void Update();
	void Draw();

private:

	float previousTime;
	float currentTime;
	float muzzlespeed;
	float cooldown;
	FlyCamera myCam;

	PxFoundation* g_PhysicsFoundation;
	PxPhysics* g_Physics;
	PxScene* g_PhysicsScene;
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;
	PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
	PxMaterial* g_PhysicsMaterial;
	PxMaterial* g_boxMaterial;
	PxCooking* g_PhysicsCooker;
	std::vector<PxRigidActor*> g_PhysXActors;
	std::vector<PxArticulation*> g_PhysXActorsRagDolls;

	void SetUpPhysX();
	void SetUpVisualDebugger();
	void SetUpEnvironment();
	void addWidget(PxShape* shape, PxRigidActor* actor);
	void addBox(PxShape* pShape, PxRigidActor* actor);
	void addSphere(PxShape* pShape, PxRigidActor* actor);
	void addCapsule(PxShape* pShape, PxRigidActor* actor);
	void FireBall();
	PxArticulation* makeRagdoll(PxPhysics* g_Physics, RagdollNode** nodeArray, PxTransform worldPos, float scaleFactor, PxMaterial* ragdollMaterial);
};

