#include "stdafx.h"
#include "SpikyMaterial.h"

SpikyMaterial::SpikyMaterial()
	: Material(L"Effects/SpikyShader.fx")
{

}

void SpikyMaterial::SetDiffuseTexture(const std::wstring&)
{
}

void SpikyMaterial::InitializeEffectVariables()
{
}

