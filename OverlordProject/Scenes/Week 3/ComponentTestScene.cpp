#include "stdafx.h"
#include "ComponentTestScene.h"

#include "Prefabs/SpherePrefab.h"

void ComponentTestScene::Initialize()
{
	auto& physx = PxGetPhysics();
	auto pBouncyMaterial = physx.createMaterial(.5f, .5f, 1.f);

	//Ground Plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	//Red Sphere (Group 0) & (Ignore Group 1 & 2)
	auto pSphereRed = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Red });
	AddChild(pSphereRed);

	pSphereRed->GetTransform()->Translate(0, 30.f, 0);

	auto pRigidBody = pSphereRed->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group0);
	pRigidBody->SetCollisionIgnoreGroups(CollisionGroup::Group1 | CollisionGroup::Group2);

	//Green Sphere (Group 1)
	auto pSphereGreen = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Green });
	AddChild(pSphereGreen);

	pSphereGreen->GetTransform()->Translate(0, 20.f, 0);

	pRigidBody = pSphereGreen->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group1);
	pRigidBody->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ, false);

	//Green Sphere (Group 2)
	auto pSphereBlue = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Blue });
	AddChild(pSphereBlue);

	pSphereBlue->GetTransform()->Translate(0, 10.f, 0);

	pRigidBody = pSphereBlue->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group2);
}
