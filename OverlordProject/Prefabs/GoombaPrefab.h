#pragma once
class GoombaPrefab final : public GameObject
{
public:
	GoombaPrefab(float x, float y, float z, GameScene* currentScene);
	~GoombaPrefab() override = default;

	GoombaPrefab(const GoombaPrefab& other) = delete;
	GoombaPrefab(GoombaPrefab&& other) noexcept = delete;
	GoombaPrefab& operator=(const GoombaPrefab& other) = delete;
	GoombaPrefab& operator=(GoombaPrefab&& other) noexcept = delete;

	bool IsRemoved() { return m_RemoveGoomba; };
	bool IsHit() { return m_GotHit; };
	void SetHit(bool state) { m_GotHit = state; };
	bool GetDoOnce() { return m_DoOnce; };
	void SetDoOnce(bool state) { m_DoOnce = state; };

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	GameScene* m_pCurrentScene{};
	ParticleEmitterComponent* m_pErmitterGoomba{};
	PxMaterial* m_pDefaultMat{};
	bool m_RemoveGoomba{};
	bool m_DoOnce{ true };

	const float m_MaxRotation{ 10.f };
	const float m_RotationSpeed{ 50.f };

	float m_CurrentRotation{ 0.f };
	bool m_RotateUp{};

	float m_CurrentTranslation{};
	float m_LocationX{};
	float m_LocationY{};
	float m_LocationZ{};
	const float m_MovementSpeed{ 0.5f };
	const float m_MaxTranslation{ 1.f };
	bool m_TransformLeft{};

	bool m_GotHit{};

	FMOD::Channel* m_pChannel6{};
	FMOD::Sound* m_pSoundHit{};
};

