#include "stdafx.h"
#include "PongScene.h"
#include <iostream>

#include "Prefabs/SpherePrefab.h"
#include "Prefabs/CubePrefab.h"

void PongScene::Initialize()
{
	auto& physx = PxGetPhysics();
	//auto pBouncyMaterial = physx.createMaterial(.5f, .5f, 1.f);

	auto pNoFriction = physx.createMaterial(0.0f, 0.0f, 1.f);

	const auto pFixedCamera = new FixedCamera();
	pFixedCamera->GetTransform()->Translate(0.f, 60.f, 0.f);
	pFixedCamera->GetTransform()->Rotate(90.f, 0.f, 0.f);
	AddChild(pFixedCamera);
	SetActiveCamera(pFixedCamera->GetComponent<CameraComponent>());

	//Red Sphere (Group 0)
	m_pSphereRed = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Red });
	AddChild(m_pSphereRed);

	//Gray Cube0 (Group 1)
	m_pCube0 = new CubePrefab(XMFLOAT3{ 2.f, 1.f, 6.f }, XMFLOAT4{ Colors::LightGray });
	AddChild(m_pCube0);
	m_pCube0->GetTransform()->Translate(-40.f, 0.f, 0.f);

	//Gray Cube1 (Group 1)
	m_pCube1 = new CubePrefab(XMFLOAT3{ 2.f, 1.f, 6.f }, XMFLOAT4{ Colors::LightGray });
	AddChild(m_pCube1);
	m_pCube1->GetTransform()->Translate(40.f, 0.f, 0.f);

	m_pBodyCube0 = m_pCube0->AddComponent(new RigidBodyComponent());
	m_pBodyCube0->SetKinematic(true);
	m_pBodyCube0->AddCollider(PxBoxGeometry{ 2.f / 2.f, 1.f / 2.f, 6.f / 2.f }, *pNoFriction);

	m_pBodyCube1 = m_pCube1->AddComponent(new RigidBodyComponent());
	m_pBodyCube1->SetKinematic(true);
	m_pBodyCube1->AddCollider(PxBoxGeometry{ 2.f / 2.f, 1.f / 2.f, 6.f / 2.f }, *pNoFriction);

	m_pBodySphere = m_pSphereRed->AddComponent(new RigidBodyComponent());
	m_pBodySphere->GetPxRigidActor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	m_pBodySphere->AddCollider(PxSphereGeometry{ 1.f }, *pNoFriction);

	//Wall0
	m_pWall0 = new GameObject();
	AddChild(m_pWall0);
	m_pWall0->GetTransform()->Translate(0.f, 0.f, 25.f);

	m_pBodyWall0 = m_pWall0->AddComponent(new RigidBodyComponent());
	m_pBodyWall0->SetKinematic(true);
	m_pBodyWall0->AddCollider(PxBoxGeometry{ 45.f, 1.f, 1.f }, *pNoFriction);

	//Wall2
	m_pWall2 = new GameObject();
	AddChild(m_pWall2);
	m_pWall2->GetTransform()->Translate(0.f, 0.f, -25.f);

	m_pBodyWall0 = m_pWall2->AddComponent(new RigidBodyComponent());
	m_pBodyWall0->SetKinematic(true);
	m_pBodyWall0->AddCollider(PxBoxGeometry{ 45.f, 1.f, 1.f }, *pNoFriction);

	m_SceneContext.settings.clearColor = static_cast<XMFLOAT4>(Colors::Black);
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
}

void PongScene::Update()
{
	//Store delta and total seconds
	float deltaT = GetSceneContext().pGameTime->GetElapsed();

	XMFLOAT3 translation_0{};
	XMFLOAT3 translation_1{};
	const float movementSpeed{ 25.f * deltaT };

	if(m_pSphereRed->GetTransform()->GetWorldPosition().x > 50.f || m_pSphereRed->GetTransform()->GetWorldPosition().x < -50.f)
	{
		m_pSphereRed->GetTransform()->Translate(0.f, 0.f, 0.f);
		m_DoOnce = true;
	}

	if(m_SceneContext.pInput->IsKeyboardKey(InputState::down, VK_UP))
	{
		translation_0.z += movementSpeed;
	}

	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, VK_DOWN))
	{
		translation_0.z -= movementSpeed;
	}

	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'R'))
	{
		translation_1.z += movementSpeed;
	}

	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'F'))
	{
		translation_1.z -= movementSpeed;
	}

	if(m_SceneContext.pInput->IsKeyboardKey(InputState::released, VK_SPACE) && m_DoOnce)
	{
		m_pBodySphere->AddForce(XMFLOAT3{ 1500.f, 0.f, 1500.f }, PxForceMode::eFORCE);
		m_DoOnce = false;
	}

	//Calculate new position 
	auto currPos_0 = XMLoadFloat3(&m_pCube0->GetTransform()->GetPosition());
	auto newPos_0 = XMVectorAdd(currPos_0, XMLoadFloat3(&translation_0));
	XMStoreFloat3(&translation_0, newPos_0);

	//Calculate new position 
	auto currPos_1 = XMLoadFloat3(&m_pCube1->GetTransform()->GetPosition());
	auto newPos_1 = XMVectorAdd(currPos_1, XMLoadFloat3(&translation_1));
	XMStoreFloat3(&translation_1, newPos_1);

	//Translate Box
	m_pCube0->GetTransform()->Translate(translation_0.x, translation_0.y, translation_0.z);

	//Translate Box
	m_pCube1->GetTransform()->Translate(translation_1.x, translation_1.y, translation_1.z);
}
