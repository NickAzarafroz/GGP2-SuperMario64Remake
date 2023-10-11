#include "stdafx.h"
#include "CoinPrefab.h"
#include "Materials/DiffuseMaterial.h"
#include "Character.h"

CoinPrefab::CoinPrefab(float x, float y, float z, float rotationSpeed, GameScene* currentScene)
{
	GetTransform()->Translate(x, y, z);
	m_pCurrentScene = currentScene;
	m_RotationSpeed = rotationSpeed;
}

void CoinPrefab::Initialize(const SceneContext&)
{
	m_pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,1.f,0.f };
	settings.minSize = 0.1f;
	settings.maxSize = 0.3f;
	settings.minEnergy = 0.3f;
	settings.maxEnergy = 0.5f;
	settings.minScale = 0.1f;
	settings.maxScale = 0.3f;
	settings.minEmitterRadius = .1f;
	settings.maxEmitterRadius = .6f;
	settings.color = { 1.f,1.f,1.f, .6f };

	DiffuseMaterial* pMaterialCoin = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMaterialCoin->SetDiffuseTexture(L"Textures/Mario_Coin_Color.png");

	const auto pMeshCoinModel = new ModelComponent(L"Meshes/CoinModel/coin.ovm");
	pMeshCoinModel->SetMaterial(pMaterialCoin);

	//Create RigidBody
	const auto pRigidBody = new RigidBodyComponent();

	const auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/CoinModel/coin.ovpc");
	PxConvexMeshGeometry pConvexMeshGeo{ pConvexMesh };
	pConvexMeshGeo.scale = PxMeshScale{ PxVec3(0.05f, 0.05f, 0.05f) };

	pRigidBody->AddCollider(pConvexMeshGeo, *m_pDefaultMat, true);
	pRigidBody->SetKinematic(true);

	m_pErmitterCoins = AddComponent(new ParticleEmitterComponent(L"Textures/Coin.png", settings, 20));
	AddComponent(pMeshCoinModel);
	AddComponent(pRigidBody);
	GetTransform()->Scale(0.05f, 0.05f, 0.05f);

	SetOnTriggerCallBack([=](GameObject*, GameObject* pOther, PxTriggerAction action)
		{
			if (action == PxTriggerAction::ENTER && pOther->GetTag() == L"Mario")
			{
				if (!m_RemoveCoin)
				{
					m_RemoveCoin = true;
				}
			}
		});
}

void CoinPrefab::Update(const SceneContext& sceneContext)
{
	m_Rotation += sceneContext.pGameTime->GetElapsed() * m_RotationSpeed;
	m_Rotation = fmodf(m_Rotation, 360.0f); // Keep the rotation within the range 0-360 degrees
	GetTransform()->Rotate(0.f, m_Rotation, 0.f);

	if(m_RemoveCoin)
	{
		m_pCurrentScene->RemoveChild(this, true);
	}
}
