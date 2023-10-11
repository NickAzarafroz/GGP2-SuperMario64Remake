#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_1::Initialize()
{
    ColorMaterial* pColorMat = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	GameObject* pRoot = new GameObject();
    AddChild(pRoot);
	m_pBone0 = new BoneObject(pColorMat, 15.f);
	pRoot->AddChild(m_pBone0);
	m_pBone1 = new BoneObject(pColorMat, 15.f);
	m_pBone0->AddBone(m_pBone1);
}

void SoftwareSkinningScene_1::Update()
{
    const float rotationSpeed = 45.0f; // degrees per second
    const float maxRotationAngle = 45.0f; // degrees
    const float deltaTime = m_SceneContext.pGameTime->GetElapsed();

    m_BoneRotation += m_RotationSpin * rotationSpeed * deltaTime;

    if (m_BoneRotation > maxRotationAngle)
    {
        m_BoneRotation = maxRotationAngle;
        m_RotationSpin = -1;
    }
    else if (m_BoneRotation < -maxRotationAngle)
    {
        m_BoneRotation = -maxRotationAngle;
        m_RotationSpin = 1;
    }

	m_pBone0->GetTransform()->Rotate(0, 0, m_BoneRotation);
	m_pBone1->GetTransform()->Rotate(0, 0, -m_BoneRotation * 2.f);
}
