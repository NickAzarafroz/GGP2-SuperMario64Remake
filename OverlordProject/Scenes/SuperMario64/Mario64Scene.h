#pragma once
class CubePrefab;
class SpherePrefab;
class GoombaPrefab;
class CoinPrefab;
class Character;
class PostBloom;
class Mario64Scene : public GameScene
{
public:
	Mario64Scene() : GameScene(L"Mario64Scene") {}
	~Mario64Scene() override;

	Mario64Scene(const Mario64Scene& other) = delete;
	Mario64Scene(Mario64Scene&& other) noexcept = delete;
	Mario64Scene& operator=(const Mario64Scene& other) = delete;
	Mario64Scene& operator=(Mario64Scene&& other) noexcept = delete;

	void RemoveNextMarioIcon();

protected:
	void Initialize() override;
	void Update() override;
	void PostDraw() override;
	void OnGUI() override;

private:
	void InitMario();
	void InitMap();

	//Character Input
	//-----------------------------
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump
	};
	//-----------------------------

	//Mario (Player)
	//---------------------------
	Character* m_pMarioCharacter{};
	GameObject* m_pMarioObject{};
	GameObject* m_pMarioIconSprite{};
	std::vector<GameObject*> m_pMarioIconSprites{};
	ParticleEmitterComponent* m_pErmitterWalk{};
	//----------------------------

	//Goomba (Enemy)
	//---------------------------
	struct GoombaData
	{
		float x;
		float y;
		float z;
	};
	std::vector<GoombaData> m_GoombaDates;
	std::vector<GoombaPrefab*> m_pGoombas;
	//---------------------------

	//Coin (Item)
	//----------------------------
	struct CoinData
	{
		float x;
		float y;
		float z;
		float rotationSpeed;
	};
	std::vector<CoinData> m_CoinDates;
	std::vector<CoinPrefab*> m_pCoins;
	GameObject* m_pCoinIconSprite{};
	int m_AmountOfCoins{};
	//----------------------------

	//Ball (Obstacle)
	//-----------------------------
	SpherePrefab* m_pBall{};

	//Platform (Obstacle)
	CubePrefab* m_pPlatform{};

	//Animation
	//------------------------------
	ModelAnimator* pAnimator{};
	int m_AnimationClipId{ 0 };
	float m_AnimationSpeed{ 1.f };
	char** m_ClipNames{};
	UINT m_ClipCount{};
	//------------------------------

	//Material
	PxMaterial* m_pDefaultMat{};

	//Physics
	PxPhysics* m_pPhysics{};

	//Shadow
	bool m_DrawShadowMap{ false };
	float m_ShadowMapScale{ 0.3f };

	//Sounds
	FMOD::Channel* m_pChannel0{};
	FMOD::Channel* m_pChannel1{};
	FMOD::Channel* m_pChannel2{};
	FMOD::Channel* m_pChannel3{};
	FMOD::Channel* m_pChannel4{};
	FMOD::Channel* m_pChannel5{};
	FMOD::Channel* m_pChannel7{};

	FMOD::Sound* m_pSoundLevel{};
	FMOD::Sound* m_pSoundJump{};
	FMOD::Sound* m_pSoundRun{};
	FMOD::Sound* m_pSoundCoinCollected{};
	FMOD::Sound* m_pSoundGoombaDeath{};
	FMOD::Sound* m_pSoundFinish{};
	FMOD::Sound* m_pSoundDeath{};

	//PostProcessing
	PostBloom* m_pPostBloom{};

	//Font
	SpriteFont* m_pFont{};
	std::string m_Text{ "x 0/7" };
	XMFLOAT2 m_TextPosition{1220.f, 40.f};
	XMFLOAT4 m_TextColor{ 1.f,1.f,1.f,1.f };

	//InGameMenu
	GameObject* m_pInGameMenuSprite{};

	//SkyBox
	GameObject* m_pSkyBox{};

	//Delay
	float m_Delay{};
	float m_DelayEnd{};

	bool m_DoOnce{ true };

	bool m_PlayOnce{ true };
};

