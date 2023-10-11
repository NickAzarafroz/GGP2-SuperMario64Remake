#pragma once
class CoinPrefab final : public GameObject
{
public:
	CoinPrefab(float x, float y, float z, float rotationSpeed, GameScene* currentScene);
	~CoinPrefab() override = default;

	CoinPrefab(const CoinPrefab& other) = delete;
	CoinPrefab(CoinPrefab&& other) noexcept = delete;
	CoinPrefab& operator=(const CoinPrefab& other) = delete;
	CoinPrefab& operator=(CoinPrefab&& other) noexcept = delete;

	bool IsRemoved() { return m_RemoveCoin; };
	bool GetDoOnce() { return m_DoOnce; };
	void SetDoOnce(bool state) { m_DoOnce = state; };

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	GameScene* m_pCurrentScene{};
	ParticleEmitterComponent* m_pErmitterCoins{};
	PxMaterial* m_pDefaultMat{};
	bool m_RemoveCoin{};
	float m_Rotation{};
	float m_RotationSpeed{};
	bool m_DoOnce{ true };
};

