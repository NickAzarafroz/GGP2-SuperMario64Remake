#include "stdafx.h"
#include "SpikyScene.h"
#include <Materials/SpikyMaterial.h>

void SpikyScene::Initialize()
{
	//Create Spike Ball
	m_pSpike = new GameObject();
	AddChild(m_pSpike);

	//Create new instance of a certain Material
	m_pMaterial = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();

	//Create model
	const auto pModel = new ModelComponent(L"Meshes/OctaSphere.ovm");

	//Assign the color material to a model
	pModel->SetMaterial(m_pMaterial);

	//Attach Model component to GameObject
	m_pSpike->AddComponent<ModelComponent>(pModel);

	//Increase the scale
	m_pSpike->GetTransform()->Scale(15.f);

	//Disable DebugGrid + Enable OnGui
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;
}

void SpikyScene::Update()
{
	const float rotationSpeed = 20.0f; // degrees per second
	const float deltaTime = m_SceneContext.pGameTime->GetElapsed();

	m_SpikeRotation += rotationSpeed * deltaTime;

	m_pSpike->GetTransform()->Rotate(0.f, m_SpikeRotation, 0.f);

}

void SpikyScene::OnGUI()
{
	m_pMaterial->DrawImGui();
}
