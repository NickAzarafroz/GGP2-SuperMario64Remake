#include "stdafx.h"
#include "GoombaPrefab.h"
#include <Materials/DiffuseMaterial.h>
#include "Character.h"
#include <Scenes/SuperMario64/Mario64Scene.h>

GoombaPrefab::GoombaPrefab(float x, float y, float z, GameScene* currentScene)
{
	GetTransform()->Translate(x, y, z);
	m_LocationX = x;
	m_LocationY = y;
	m_LocationZ = z;
	m_pCurrentScene = currentScene;
}

void GoombaPrefab::Initialize(const SceneContext&)
{
	m_pDefaultMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	DiffuseMaterial* pMaterialGoomba = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMaterialGoomba->SetDiffuseTexture(L"Textures/goomba_grp.png");

	const auto pMeshGoombaModel = new ModelComponent(L"Meshes/GoombaModel/Goomba.ovm");
	pMeshGoombaModel->SetMaterial(pMaterialGoomba);

	//Create RigidBody
	const auto pRigidBody = new RigidBodyComponent();

	auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/GoombaModel/Goomba.ovpc");
	PxConvexMeshGeometry pConvexGeo{ pConvexMesh };
	pConvexGeo.scale = PxMeshScale{ PxVec3(0.025f, 0.025f, 0.025f) };

	pRigidBody->AddCollider(pConvexGeo, *m_pDefaultMat, true);
	pRigidBody->SetKinematic(true);

	AddComponent(pRigidBody);
	AddComponent(pMeshGoombaModel);

	GetTransform()->Scale(0.025f, 0.025f, 0.025f);
	GetTransform()->Translate(-5.f, 9.f, -14.f);

	auto pFmod = SoundManager::Get()->GetSystem();

	pFmod->createStream("Resources/Sounds/Hit.mp3", FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &m_pSoundHit);

	SetOnTriggerCallBack([=](GameObject*, GameObject* pOther, PxTriggerAction action)
		{
			if (action == PxTriggerAction::ENTER && pOther->GetTag() == L"Mario")
			{
				Character* pMarioCharacter = dynamic_cast<Character*>(pOther);
				if (pMarioCharacter != nullptr)
				{
					if (pMarioCharacter->GetFallSpeed() < -1.f && !m_RemoveGoomba)
					{
						pMarioCharacter->SetTotalVelocity(0.f, 15.f, 0.f);
						m_RemoveGoomba = true;
					}
					else
					{
						pFmod->playSound(m_pSoundHit, nullptr, false, &m_pChannel6);
						m_pChannel6->setMode(FMOD_LOOP_OFF);
						m_pChannel6->set3DMinMaxDistance(0.f, 0.f);


						Mario64Scene* pMarioScene = dynamic_cast<Mario64Scene*>(m_pCurrentScene);
						pMarioScene->RemoveNextMarioIcon();
						pMarioCharacter->SetMoveSpeed(0.f);
					}
				}
			}
		});
}

void GoombaPrefab::Update(const SceneContext& sceneContext)
{
	if (m_RotateUp)
	{
		m_CurrentRotation += sceneContext.pGameTime->GetElapsed() * m_RotationSpeed;

		if(m_TransformLeft)
		{
			GetTransform()->Rotate(0.f, 180.f, m_CurrentRotation);
		}
		else
		{
			GetTransform()->Rotate(0.f, 0.f, m_CurrentRotation);
		}
	}
	else
	{
		m_CurrentRotation -= sceneContext.pGameTime->GetElapsed() * m_RotationSpeed;

		if (m_TransformLeft)
		{
			GetTransform()->Rotate(0.f, 180.f, m_CurrentRotation);
		}
		else
		{
			GetTransform()->Rotate(0.f, 0.f, m_CurrentRotation);
		}
	}

	// Check if the maximum rotation angle is reached
	if (m_CurrentRotation >= m_MaxRotation)
	{
		m_RotateUp = false;
		m_CurrentRotation = m_MaxRotation;
	}
	else if (m_CurrentRotation <= -m_MaxRotation)
	{
		m_RotateUp = true;
		m_CurrentRotation = -m_MaxRotation;
	}


	if(m_TransformLeft)
	{
		m_CurrentTranslation -= sceneContext.pGameTime->GetElapsed() * m_MovementSpeed;
		GetTransform()->Translate(m_LocationX + m_CurrentTranslation, m_LocationY, m_LocationZ);
	}
	else
	{
		m_CurrentTranslation += sceneContext.pGameTime->GetElapsed() * m_MovementSpeed;
		GetTransform()->Translate(m_LocationX + m_CurrentTranslation, m_LocationY, m_LocationZ);
	}

	if (m_CurrentTranslation >= m_MaxTranslation)
	{
		m_TransformLeft = true;
		m_CurrentTranslation = m_MaxTranslation;
	}
	else if (m_CurrentTranslation <= -m_MaxTranslation)
	{
		m_TransformLeft = false;
		m_CurrentTranslation = -m_MaxTranslation;
	}

	

	if(m_RemoveGoomba)
	{
		m_pCurrentScene->RemoveChild(this, true);
	}
}
