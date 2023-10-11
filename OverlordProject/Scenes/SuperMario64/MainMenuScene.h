#pragma once
class MainMenuScene : public GameScene
{
public:
	MainMenuScene() : GameScene(L"Mario64Scene") {}
	~MainMenuScene() override = default;

	MainMenuScene(const MainMenuScene& other) = delete;
	MainMenuScene(MainMenuScene&& other) noexcept = delete;
	MainMenuScene& operator=(const MainMenuScene& other) = delete;
	MainMenuScene& operator=(MainMenuScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;

private:
	GameObject* m_pMainMenuSprite{};
};

