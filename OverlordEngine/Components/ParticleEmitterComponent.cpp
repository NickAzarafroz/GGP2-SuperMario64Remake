#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	//TODO_W9(L"Implement Destructor")
	delete[] m_ParticlesArray;
	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Initialize")
	if(!m_pParticleMaterial)
	{
		m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();
	}

	CreateVertexBuffer(sceneContext);

	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement CreateVertexBuffer")
	if(m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
	}

	//Create Dynamic Buffer
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(VertexParticle) * m_ParticleCount;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	HANDLE_ERROR(sceneContext.d3dContext.pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pVertexBuffer));
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Update")
	float particleInterval = (m_EmitterSettings.maxEnergy + m_EmitterSettings.minEnergy) / 2.0f;
	particleInterval /= static_cast<float>(m_ParticleCount);

	m_LastParticleSpawn += sceneContext.pGameTime->GetElapsed();

	m_ActiveParticles = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource{};
	sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	auto pBuffer = static_cast<VertexParticle*>(mappedResource.pData);

	float cachedElapsedTime = sceneContext.pGameTime->GetElapsed();

	for(UINT i{}; i < m_ParticleCount; ++i)
	{
		if(m_ParticlesArray[i].isActive)
		{
			UpdateParticle(m_ParticlesArray[i], cachedElapsedTime);
		}

		if(!m_ParticlesArray[i].isActive && m_LastParticleSpawn >= particleInterval)
		{
			SpawnParticle(m_ParticlesArray[i]);
		}

		if(m_ParticlesArray[i].isActive)
		{
			pBuffer[m_ActiveParticles] = m_ParticlesArray[i].vertexInfo;

			++m_ActiveParticles;
		}
	}

	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	//TODO_W9(L"Implement UpdateParticle")
	if (!p.isActive) return;

	p.currentEnergy -= elapsedTime;

	if (p.currentEnergy < 0.0f)
	{
		p.isActive = false;
		return;
	}

	float lifePercent = p.currentEnergy / p.totalEnergy;

	XMFLOAT3 velocity = m_EmitterSettings.velocity;
	p.vertexInfo.Position.x += velocity.x * elapsedTime;
	p.vertexInfo.Position.y += velocity.y * elapsedTime;
	p.vertexInfo.Position.z += velocity.z * elapsedTime;

	p.vertexInfo.Color = m_EmitterSettings.color;
	p.vertexInfo.Color.w *= lifePercent * 2.0f;

	p.vertexInfo.Size = std::lerp(p.initialSize * p.sizeChange, p.initialSize, lifePercent);
}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	//TODO_W9(L"Implement SpawnParticle")
	p.isActive = true;

	p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	p.currentEnergy = p.totalEnergy;

	XMVECTOR randomDirection = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMMATRIX randomRotationMatrix = XMMatrixRotationRollPitchYaw(
		MathHelper::randF(-XM_PI, XM_PI), // Random Yaw
		MathHelper::randF(-XM_PI, XM_PI), // Random Pitch
		MathHelper::randF(-XM_PI, XM_PI)  // Random Roll
	);

	randomDirection = XMVector3TransformNormal(randomDirection, randomRotationMatrix);

	float randomDistance = MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius);

	XMVECTOR initialPosition = randomDirection * randomDistance;
	const XMFLOAT3 worldPos{ GetGameObject()->GetTransform()->GetWorldPosition() };
	XMStoreFloat3(&p.vertexInfo.Position, XMVectorAdd(XMVectorScale(randomDirection, randomDistance), XMLoadFloat3(&worldPos)));

	p.vertexInfo.Size = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.initialSize = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);
	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement PostDraw")
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture->GetShaderResourceView());

	const MaterialTechniqueContext& techniqueContext = m_pParticleMaterial->GetTechniqueContext();

	sceneContext.d3dContext.pDeviceContext->IASetInputLayout(techniqueContext.pInputLayout);
	sceneContext.d3dContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(VertexParticle);
	UINT offset = 0;
	sceneContext.d3dContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techniqueDesc{};
	techniqueContext.pTechnique->GetDesc(&techniqueDesc);

	for (UINT i = 0; i < techniqueDesc.Passes; i++)
	{
		techniqueContext.pTechnique->GetPassByIndex(i)->Apply(0, sceneContext.d3dContext.pDeviceContext);

		sceneContext.d3dContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}

}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}