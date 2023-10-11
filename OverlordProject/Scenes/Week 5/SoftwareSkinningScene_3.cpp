#include "stdafx.h"
#include "SoftwareSkinningScene_3.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_3::Initialize()
{
	ColorMaterial* pColorMat = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	GameObject* pRoot = new GameObject();
	AddChild(pRoot);
	m_pBone0 = new BoneObject(pColorMat, 15.f);
	pRoot->AddChild(m_pBone0);
	m_pBone1 = new BoneObject(pColorMat, 15.f);
	m_pBone0->AddBone(m_pBone1);

	MeshDrawComponent* pDrawComponent = new MeshDrawComponent(24, true);
	m_pBone0->CalculateBindPose();

	GameObject* pBoxDrawer = new GameObject();
	AddChild(pBoxDrawer);
	m_pMeshDrawer = pBoxDrawer->AddComponent(pDrawComponent);

	InitializeVertices(15.f);
}

void SoftwareSkinningScene_3::Update()
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

	m_pMeshDrawer->RemoveTriangles();

	for (size_t i{}; i < m_SkinnedVertices.size(); i += 4)
	{
		QuadPosNormCol quad;

		quad.Vertex1.Position = m_SkinnedVertices[i].transformedVertex.Position;
		quad.Vertex1.Normal = m_SkinnedVertices[i].transformedVertex.Normal;
		quad.Vertex1.Color = m_SkinnedVertices[i].transformedVertex.Color;

		quad.Vertex2.Position = m_SkinnedVertices[i + 1].transformedVertex.Position;
		quad.Vertex2.Normal = m_SkinnedVertices[i + 1].transformedVertex.Normal;
		quad.Vertex2.Color = m_SkinnedVertices[i + 1].transformedVertex.Color;

		quad.Vertex3.Position = m_SkinnedVertices[i + 2].transformedVertex.Position;
		quad.Vertex3.Normal = m_SkinnedVertices[i + 2].transformedVertex.Normal;
		quad.Vertex3.Color = m_SkinnedVertices[i + 2].transformedVertex.Color;

		quad.Vertex4.Position = m_SkinnedVertices[i + 3].transformedVertex.Position;
		quad.Vertex4.Normal = m_SkinnedVertices[i + 3].transformedVertex.Normal;
		quad.Vertex4.Color = m_SkinnedVertices[i + 3].transformedVertex.Color;

		m_pMeshDrawer->AddQuad(quad);
	}

	const XMMATRIX bone0BindPose = XMLoadFloat4x4(&m_pBone0->GetBindPose());
	const XMMATRIX bone0WorldPos = XMLoadFloat4x4(&m_pBone0->GetTransform()->GetWorld());

	const XMMATRIX bone1BindPose = XMLoadFloat4x4(&m_pBone1->GetBindPose());
	const XMMATRIX bone1WorldPos = XMLoadFloat4x4(&m_pBone1->GetTransform()->GetWorld());

	auto bone0Transforms = XMMatrixMultiply(bone0BindPose, bone0WorldPos);
	auto bone1Transforms = XMMatrixMultiply(bone1BindPose, bone1WorldPos);

	for (size_t i{}; i < m_SkinnedVertices.size(); ++i)
	{
		XMFLOAT3 originalPos = m_SkinnedVertices[i].originalVertex.Position;
		//XMFLOAT3 transformedPos;

		float blendWeight0 = m_SkinnedVertices[i].blendWeight0;
		float blendWeight1 = m_SkinnedVertices[i].blendWeight1;

		//XMMATRIX boneTransform{};

		//XMMATRIX boneTransform;
		/*if (i < 24)
		{
			boneTransform = bone0Transforms;
		}
		else
		{
			boneTransform = bone1Transforms;
		}*/

		XMVECTOR position0 = XMVector3Transform(XMLoadFloat3(&originalPos), bone0Transforms);
		XMVECTOR position1 = XMVector3Transform(XMLoadFloat3(&originalPos), bone1Transforms);

		//XMVECTOR finalPosition = XMVectorLerp(position0, position1, blendWeight1 / (blendWeight0 + blendWeight1));
		XMVECTOR finalPosition = position0 * blendWeight0 + position1 * blendWeight1;

		XMStoreFloat3(&m_SkinnedVertices[i].transformedVertex.Position, finalPosition);

		//XMVECTOR originalPosVec = XMLoadFloat3(&originalPos);
		//XMVECTOR transformedPosVec = XMVector3TransformCoord(originalPosVec, boneTransform);

		//XMStoreFloat3(&transformedPos, transformedPosVec);
		//m_SkinnedVertices[i].transformedVertex.Position = transformedPos;
	}

	m_pMeshDrawer->UpdateBuffer();
}

void SoftwareSkinningScene_3::InitializeVertices(float length)
{
	auto pos = XMFLOAT3(length / 2.f, 0.f, 0.f);
	const auto offset = XMFLOAT3(length / 2.f, 2.5f, 2.5f);
	auto col = XMFLOAT4(1.f, 0.f, 0.f, 0.5f);
#pragma region BOX 1
	//FRONT
	XMFLOAT3 norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f, 0.f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f, 0.f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f, 0.f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f, 0.f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
#pragma endregion

	col = { 0.f, 1.f, 0.f, 0.5f };
	pos = { 22.5f, 0.f, 0.f };
#pragma region BOX 2
	//FRONT
	norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f, 0.5f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
#pragma endregion
}