#pragma once
struct CharacterDesc
{
	CharacterDesc(
		PxMaterial* pMaterial,
		float radius = .5f,
		float height = 2.f)
	{
		controller.setToDefault();
		controller.radius = radius;
		controller.height = height;
		controller.material = pMaterial;
	}

	float maxMoveSpeed{ 8.f };
	float maxFallSpeed{ 15.f };

	float JumpSpeed{ 12.f };

	float moveAccelerationTime{ .3f };
	float fallAccelerationTime{ .3f };

	PxCapsuleControllerDesc controller{};

	float rotationSpeed{ 30.f };

	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_Jump{ -1 };
};

class Character : public GameObject
{
public:
	Character(const CharacterDesc& characterDesc);
	~Character() override = default;

	Character(const Character& other) = delete;
	Character(Character&& other) noexcept = delete;
	Character& operator=(const Character& other) = delete;
	Character& operator=(Character&& other) noexcept = delete;

	float GetTotalYaw() { return m_TotalYaw; };
	float GetMoveSpeed() { return m_MoveSpeed; };
	float GetFallSpeed() { return m_TotalVelocity.y; };
	void SetMoveSpeed(float speed) { m_MoveSpeed = speed; };
	void SetTotalVelocity(float x, float y, float z) { m_TotalVelocity.x = x; m_TotalVelocity.y = y; m_TotalVelocity.z = z; };
	ControllerComponent* GetController() { return m_pControllerComponent; };

	void DrawImGui();

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	CameraComponent* m_pCameraComponent{};
	ControllerComponent* m_pControllerComponent{};

	CharacterDesc m_CharacterDesc;
	float m_TotalPitch{}, m_TotalYaw{};				//Total camera Pitch(X) and Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)

	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)
};

