#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* pMaterial, float length)
	: m_pMaterial{ pMaterial }, m_Length{ length }
{

}

void BoneObject::AddBone(BoneObject* pBone)
{
	pBone->GetTransform()->Translate(m_Length, 0.f, 0.f);

	this->AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{
	const XMFLOAT4X4& worldMatrix = GetTransform()->GetWorld();

	XMMATRIX invWorldMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&worldMatrix));
	XMStoreFloat4x4(&m_BindPose, invWorldMatrix);

	for(const auto& pchild : GetChildren<BoneObject>())
	{
		pchild->CalculateBindPose();
	}
}

void BoneObject::Initialize(const SceneContext&)
{
	GameObject* pEmpty = new GameObject();
	AddChild(pEmpty);

	ModelComponent* pModel = new ModelComponent(L"Meshes/Bone.ovm");
	pEmpty->AddComponent(pModel);
	pModel->SetMaterial(m_pMaterial);
	pEmpty->GetTransform()->Rotate(0.f, -90.f, 0.f);
	pEmpty->GetTransform()->Scale(m_Length, m_Length, m_Length);
}
