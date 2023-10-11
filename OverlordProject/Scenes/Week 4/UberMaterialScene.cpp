#include "stdafx.h"
#include "UberMaterialScene.h"
#include "Materials/UberMaterial.h"

void UberMaterialScene::Initialize()
{
	//Create Sphere Ball
	m_pSphere = new GameObject();
	AddChild(m_pSphere);

	//Create new instance of a certain Material
	m_pMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	m_pMaterial->SetDiffuseTexture(L"Textures/Skulls_Diffusemap.tga");
	m_pMaterial->SetNormalTexture(L"Textures/Skulls_Normalmap.tga");
	m_pMaterial->SetEnvironmentTexture(L"Textures/Sunol_Cubemap.dds");
	m_pMaterial->SetHeightTexture(L"Textures/Skulls_Heightmap.tga");

	//Create model
	const auto pModel = new ModelComponent(L"Meshes/Sphere.ovm");

	//Assign the color material to a model
	pModel->SetMaterial(m_pMaterial);

	//Attach Model component to GameObject
	m_pSphere->AddComponent<ModelComponent>(pModel);

	//Increase the scale
	m_pSphere->GetTransform()->Scale(15.f);

	//Disable DebugGrid + Enable OnGui
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;
}

void UberMaterialScene::Update()
{
	const float rotationSpeed = 40.0f; // degrees per second
	const float deltaTime = m_SceneContext.pGameTime->GetElapsed();

	m_SphereRotation += rotationSpeed * deltaTime;

	m_pSphere->GetTransform()->Rotate(0.f, m_SphereRotation, 0.f);
}

void UberMaterialScene::OnGUI()
{
	m_pMaterial->DrawImGui();
}
