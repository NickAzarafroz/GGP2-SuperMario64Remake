#pragma once
class UberMaterial final : public Material<UberMaterial>
{
public:
	UberMaterial();
	~UberMaterial() override = default;

	UberMaterial(const UberMaterial& other) = delete;
	UberMaterial(UberMaterial&& other) noexcept = delete;
	UberMaterial& operator=(const UberMaterial& other) = delete;
	UberMaterial& operator=(UberMaterial&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetNormalTexture(const std::wstring& assetFile);
	void SetEnvironmentTexture(const std::wstring& assetFile);
	void SetHeightTexture(const std::wstring& assetFile);

protected:
	void InitializeEffectVariables() override;

private:
	TextureData* m_pDiffuseTexture{ nullptr };
	TextureData* m_pNormalTexture{ nullptr };
	TextureData* m_pEnvironmentTexture{ nullptr };
	TextureData* m_pHeightTexture{ nullptr };

	bool m_UseTextureDiffuse{};
	bool m_UseTextureNormal{};
	bool m_UseTextureEnvironment{};
	bool m_UseHeightTexture{};
	bool m_FlipGreenChannel{};
	bool m_UseFresnelFalloff{};
	bool m_UseSpecularPhong{};

	float m_Shininess{};
	float m_ReflectionStrength{};
	float m_RefractionStrength{};
	float m_RefractionIndex{};
	float m_FresnelPower{};
	float m_FresnelMultiplier{};
	float m_FresnelHardness{};
	float m_OpacityIntensity{};

	XMFLOAT3 m_LightDirection{};
	XMFLOAT4 m_ColorDiffuse{};
	XMFLOAT4 m_ColorSpecular{}; 
};

