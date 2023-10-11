#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial()
	: Material(L"Effects/Ubershader.fx")
{

}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);

	SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture);
}

void UberMaterial::SetNormalTexture(const std::wstring& assetFile)
{
	m_pNormalTexture = ContentManager::Load<TextureData>(assetFile);

	SetVariable_Texture(L"gTextureNormal", m_pNormalTexture);
}

void UberMaterial::SetEnvironmentTexture(const std::wstring& assetFile)
{
	m_pEnvironmentTexture = ContentManager::Load<TextureData>(assetFile);

	SetVariable_Texture(L"gCubeEnvironment", m_pEnvironmentTexture);
}

void UberMaterial::SetHeightTexture(const std::wstring& assetFile)
{
	m_pHeightTexture = ContentManager::Load<TextureData>(assetFile);

	SetVariable_Texture(L"gTextureSpecularIntensity", m_pHeightTexture);
}

void UberMaterial::InitializeEffectVariables()
{
	m_UseTextureDiffuse = true;
	SetVariable_Scalar(L"gUseTextureDiffuse", m_UseTextureDiffuse); 
	
	m_UseTextureNormal = true;
	SetVariable_Scalar(L"gUseTextureNormal", m_UseTextureNormal);

	m_UseTextureEnvironment = true;
	SetVariable_Scalar(L"gUseEnvironmentMapping", m_UseTextureEnvironment);

	m_UseHeightTexture = true;
	SetVariable_Scalar(L"gUseTextureSpecularIntensity", m_UseHeightTexture);

	m_LightDirection = { 0.f, 1.f, -1.f };
	SetVariable_Vector(L"gLightDirection", m_LightDirection);

	m_ColorDiffuse = { 1.f, 0.f, 0.f, 1.f };
	SetVariable_Vector(L"gColorDiffuse", m_ColorDiffuse);

	m_ColorSpecular = { 0.f, 1.f, 0.f, 1.f };
	SetVariable_Vector(L"gColorSpecular", m_ColorSpecular);

	m_Shininess = 40.f;
	SetVariable_Scalar(L"gShininess", m_Shininess);

	m_FlipGreenChannel = true;
	SetVariable_Scalar(L"gFlipGreenChannel", m_FlipGreenChannel);

	m_ReflectionStrength = 1.6f;
	SetVariable_Scalar(L"gReflectionStrength", m_ReflectionStrength);

	m_RefractionStrength = 1.0f;
	SetVariable_Scalar(L"gRefractionStrength", m_RefractionStrength);

	m_RefractionIndex = 1.8f;
	SetVariable_Scalar(L"gRefractionIndex", m_RefractionIndex);

	m_UseFresnelFalloff = true;
	SetVariable_Scalar(L"gUseFresnelFalloff", m_UseFresnelFalloff);

	m_FresnelPower = 1.0f;
	SetVariable_Scalar(L"gFresnelPower", m_FresnelPower);

	m_FresnelMultiplier = 1.5f;
	SetVariable_Scalar(L"gFresnelMultiplier", m_FresnelMultiplier);

	m_FresnelHardness = 2.0f;
	SetVariable_Scalar(L"gFresnelHardness", m_FresnelHardness);

	m_OpacityIntensity = 1.0f;
	SetVariable_Scalar(L"gOpacityIntensity", m_OpacityIntensity);

	m_UseSpecularPhong = true;
	SetVariable_Scalar(L"gUseSpecularPhong", m_UseSpecularPhong);
}
