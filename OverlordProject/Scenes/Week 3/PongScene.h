#pragma once
class PongScene : public GameScene
{
public:
	PongScene() : GameScene(L"PongScene") {}
	~PongScene() override = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;

private:
	GameObject* m_pSphereRed{ nullptr };
	GameObject* m_pCube0{ nullptr };
	GameObject* m_pCube1{ nullptr };

	GameObject* m_pWall0{ nullptr };
	GameObject* m_pWall1{ nullptr };
	GameObject* m_pWall2{ nullptr };
	GameObject* m_pWall3{ nullptr };

	RigidBodyComponent* m_pBodySphere{ nullptr };
	RigidBodyComponent* m_pBodyCube0{ nullptr };
	RigidBodyComponent* m_pBodyCube1{ nullptr };

	RigidBodyComponent* m_pBodyWall0{ nullptr };
	RigidBodyComponent* m_pBodyWall1{ nullptr };

	bool m_DoOnce{ true };
};

