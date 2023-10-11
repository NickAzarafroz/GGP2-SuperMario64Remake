#include "stdafx.h"
#include "SkyBoxMaterial.h"

SkyBoxMaterial::SkyBoxMaterial()
	: Material(L"Effects/SkyBox.fx")
{

}

void SkyBoxMaterial::SetCubeTexture(const std::wstring& assetFile)
{
	m_pCubeTexture = ContentManager::Load<TextureData>(assetFile);

	SetVariable_Texture(L"m_CubeMap", m_pCubeTexture);
}

void SkyBoxMaterial::InitializeEffectVariables()
{

}
