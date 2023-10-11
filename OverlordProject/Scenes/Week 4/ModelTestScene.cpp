#include "stdafx.h"
#include "ModelTestScene.h"
#include <Materials/DiffuseMaterial.h>

void ModelTestScene::Initialize()
{
	//Physics
	auto& physx = PxGetPhysics();
	auto pNoFriction = physx.createMaterial(0.0f, 0.0f, 1.f);

	//Ground Plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pNoFriction);

	//Create Chair
	m_pChair = new GameObject();
	AddChild(m_pChair);

	//Create new instance of a certain Material
	DiffuseMaterial* pMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMaterial->SetDiffuseTexture(L"Textures/Chair_Dark.dds");

	//Create Model
	const auto pModel = new ModelComponent(L"Meshes/Chair.ovm");

	//Assign the color material to a model
	pModel->SetMaterial(pMaterial);

	//Attach Model component to GameObject
	m_pChair->AddComponent<ModelComponent>(pModel);

	//Create RigidBody
	const auto pRigidBody = new RigidBodyComponent();
	m_pChair->AddComponent<RigidBodyComponent>(pRigidBody);

	const auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Chair.ovpc");
	pRigidBody->AddCollider(PxConvexMeshGeometry{ pConvexMesh }, *pNoFriction);

	m_pChair->GetTransform()->Translate(0.f, 5.f, 0.f);
}
