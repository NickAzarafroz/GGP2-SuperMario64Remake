#include "stdafx.h"
#include "Mario64Scene.h"
#include <Materials/DiffuseMaterial.h>
#include <Prefabs/Character.h>
#include <Materials/ColorMaterial.h>
#include <Materials/DiffuseMaterial_Skinned.h>
#include <Prefabs/CoinPrefab.h>
#include <Prefabs/GoombaPrefab.h>
#include <Prefabs/SpherePrefab.h>
#include <Prefabs/CubePrefab.h>
#include <Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h>
#include <Materials/Shadow/DiffuseMaterial_Shadow.h>
#include <Materials/Post/PostBloom.h>
#include <Materials/SkyBoxMaterial.h>

Mario64Scene::~Mario64Scene()
{
	for (UINT i{ 0 }; i < m_ClipCount; ++i)
	{
		delete m_ClipNames[i];
	}

	delete[] m_ClipNames;
}

void Mario64Scene::RemoveNextMarioIcon()
{
	if (!m_pMarioIconSprites.empty())
	{
		RemoveChild(m_pMarioIconSprites.back(), true);
		m_pMarioIconSprites.pop_back();
	}
}

void Mario64Scene::Initialize()
{
	//Setting 
	//----------------------------------------------
	m_SceneContext.settings.enableOnGUI = false;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;

	//Post
	m_pPostBloom = MaterialManager::Get()->CreateMaterial<PostBloom>();
	AddPostProcessingEffect(m_pPostBloom);

	//Light
	m_SceneContext.pLights->SetDirectionalLight({ -55.6139526f,66.1346436f,-31.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//Text
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");

	//Sound
	auto pFmod = SoundManager::Get()->GetSystem();
	pFmod->createStream("Resources/Sounds/LevelMusic.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &m_pSoundLevel);
	pFmod->createStream("Resources/Sounds/Jump.mp3", FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &m_pSoundJump);
	pFmod->createStream("Resources/Sounds/Run.mp3", FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &m_pSoundRun);
	pFmod->createStream("Resources/Sounds/CoinCollected.mp3", FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &m_pSoundCoinCollected);
	pFmod->createStream("Resources/Sounds/GoombaDeath.mp3", FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &m_pSoundGoombaDeath);
	pFmod->createStream("Resources/Sounds/Finish.mp3", FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &m_pSoundFinish);
	pFmod->createStream("Resources/Sounds/Death.mp3", FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &m_pSoundDeath);
	//----------------------------------------------

	//Default Material
	//--------------------------------------------------------------------------------
	m_pDefaultMat = PxGetPhysics().createMaterial(0.1f, 0.1f, 0.1f);
	GameSceneExt::CreatePhysXGroundPlane(*this, m_pDefaultMat);
	//--------------------------------------------------------------------------------

	//Character
	//--------------------------------------------------------------------------------
	
	//Apply Movement Input and Material to character (Mario)
	CharacterDesc marioDesc{ m_pDefaultMat };
	marioDesc.actionId_MoveForward = CharacterMoveForward;
	marioDesc.actionId_MoveBackward = CharacterMoveBackward;
	marioDesc.actionId_MoveLeft = CharacterMoveLeft;
	marioDesc.actionId_MoveRight = CharacterMoveRight;
	marioDesc.actionId_Jump = CharacterJump;

	marioDesc.controller.height *= 0.3f;

	marioDesc.maxMoveSpeed = 6.f;
	marioDesc.maxFallSpeed = 8.f;

	//Assign character and transform to requiered position
	m_pMarioCharacter = AddChild(new Character(marioDesc));
	m_pMarioCharacter->GetTransform()->Translate(-28.f, 13.f, -28.f);
	//--------------------------------------------------------------------------------

	//Create the map with Materials + collision + position 
	//------------------------------------------------------
	InitMap();
	//------------------------------------------------------

	//Create SkyBox
	m_pSkyBox = AddChild(new GameObject());
	SkyBoxMaterial* pSkyboxMaterial = MaterialManager::Get()->CreateMaterial<SkyBoxMaterial>();
	pSkyboxMaterial->SetCubeTexture(L"Textures/SkyBox.dds");
	ModelComponent* pSkymodel = new ModelComponent(L"Meshes/Box.ovm");
	pSkymodel->SetMaterial(pSkyboxMaterial);
	m_pSkyBox->AddComponent(pSkymodel);
	


	//Create Mario Model
	//--------------------------------------------------------------------------------
	m_pMarioObject = AddChild(new GameObject());
	m_pMarioCharacter->SetTag(L"Mario");
	//Set Materials Mario + Animations + Position (Startposition of Mario in the game)
	InitMario();
	//--------------------------------------------------------------------------------

	//Create Mario Icons
	//-------------------------------------------------------------------------------------------------------------------------
	m_pMarioIconSprites.resize(3);

	for(size_t i{}; i < m_pMarioIconSprites.size(); ++i)
	{
		m_pMarioIconSprites[i] = AddChild(new GameObject());
		m_pMarioIconSprites[i]->AddComponent(new SpriteComponent(L"Textures/MarioIcon.png", {0.5f,0.5f}));

		m_pMarioIconSprites[i]->GetTransform()->Translate(50.f + (50.f * i), 50.f, .9f);
		m_pMarioIconSprites[i]->GetTransform()->Scale(2.f, 2.f, 2.f);
	}
	//--------------------------------------------------------------------------------------------------------------------------

	//Create Coin Icon
	m_pCoinIconSprite = AddChild(new GameObject());
	m_pCoinIconSprite->AddComponent(new SpriteComponent(L"Textures/CoinUI.png", { 0.5f,0.5f }));
	m_pCoinIconSprite->GetTransform()->Translate(1200.f, 50.f, 0.9f);
	m_pCoinIconSprite->GetTransform()->Scale(2.f, 2.f, 2.f);

	//Create Goombas
	//---------------------------------------------------------------------------------
	m_GoombaDates =
	{
		{ -10.f, 8.75f, -20.f },
		{ 0.f, 8.75f, -20.f },
		{ 15.f, 12.5f, -30.f },
		{ -20.f, 12.5f, -13.f },
		{ -25.f, 12.5f, -10.f },
		{ -20.f, 12.5f, -16.f },
		{ -28.f, 12.5f, -13.f },
		{ -25.f, 13.75f, 7.f },
	};

	for (const GoombaData& data : m_GoombaDates)
	{
		GoombaPrefab* goomba = AddChild(new GoombaPrefab(data.x, data.y, data.z, this));
		m_pGoombas.push_back(goomba);
	}
	//---------------------------------------------------------------------------------

	//Create Coins
	//---------------------------------------------------------------------------------
	m_CoinDates =
	{
		{ -5.f, 9.f, -20.f, 180.f },
		{ 0.f, 9.f, -30.f, 180.f },
		{ 15.f, 13.f, -35.f, 180.f },
		{ -20.f, 13.f, -15.f, 180.f },
		{ -25.f, 16.f, 15.f, 180.f },
		{ 35.f, 20.f, 35.f, 180.f },
		{ 10.f, 31.f, 20.f, 180.f }
	};

	for (const CoinData& data : m_CoinDates)
	{
		CoinPrefab* coin = AddChild(new CoinPrefab(data.x, data.y, data.z, data.rotationSpeed, this));
		m_pCoins.push_back(coin);
	}
	//---------------------------------------------------------------------------------

	//Create Ball
	//------------------------------------------------------------------------------------
	m_pBall = AddChild(new SpherePrefab(0.75f, 10, XMFLOAT4{ Colors::Black }));
	auto pRigidBody = m_pBall->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry{ 0.5f }, *m_pDefaultMat);

	m_pBall->GetTransform()->Translate(5.f, 30.f, 20.f);
	//--------------------------------------------------------------------------------------

	//Create Platform
	//--------------------------------------------------------------------------------------
	m_pPlatform = AddChild(new CubePrefab(1.f, 1.f, 1.f, XMFLOAT4{ Colors::Brown }));
	auto pRigidBodyP = m_pPlatform->AddComponent(new RigidBodyComponent(true));
	pRigidBodyP->AddCollider(PxBoxGeometry{ 0.5f, 0.5f, 0.5f }, *m_pDefaultMat);

	m_pPlatform->GetTransform()->Translate(-11.25f, 11.5f, -5.5f);

	//Gather Clip Names
	//-----------------------------------------------------------------------------------------------
	m_ClipCount = pAnimator->GetClipCount();
	m_ClipNames = new char* [m_ClipCount];
	for (UINT i{ 0 }; i < m_ClipCount; ++i)
	{
		auto clipName = StringUtil::utf8_encode(pAnimator->GetClip(static_cast<int>(i)).name);
		const auto clipSize = clipName.size();
		m_ClipNames[i] = new char[clipSize + 1];
		strncpy_s(m_ClipNames[i], clipSize + 1, clipName.c_str(), clipSize);
	}
	//-----------------------------------------------------------------------------------------------

	//Input
	//--------------------------------------------------------------------------------------------------
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE);
	m_SceneContext.pInput->AddInputAction(inputAction);

	m_SceneContext.pCamera->GetTransform()->Translate(0.f, 1.f, -5.5f);
	m_SceneContext.pCamera->GetTransform()->Rotate(15.f, 0.f, 0.f);

	pAnimator->Play();
	//--------------------------------------------------------------------------------------------------
}

void Mario64Scene::Update()
{
	if(m_PlayOnce)
	{
		auto pFmod = SoundManager::Get()->GetSystem();
		pFmod->playSound(m_pSoundLevel, nullptr, false, &m_pChannel0);
		m_pChannel0->setMode(FMOD_LOOP_NORMAL);
		m_pChannel0->set3DMinMaxDistance(0.f, 0.f);
		m_PlayOnce = false;
	}

	m_SceneContext.pInput->CursorVisible(false);
	m_SceneContext.pInput->ForceMouseToCenter(true);

	if(m_AmountOfCoins >= 7)
	{
		m_Delay += m_SceneContext.pGameTime->GetElapsed();

		if(m_DoOnce)
		{
			m_pChannel0->stop();
			auto pFmod = SoundManager::Get()->GetSystem();
			pFmod->playSound(m_pSoundFinish, nullptr, false, &m_pChannel5);
			m_pChannel5->setMode(FMOD_LOOP_OFF);
			m_pChannel5->set3DMinMaxDistance(0.f, 0.f);
			m_DoOnce = false;
		}

		if (m_Delay >= 3.5f)
		{
			m_pChannel2->stop();
			SceneManager::Get()->NextScene();
			m_AmountOfCoins = 0;
			m_Delay = 0;
			m_DoOnce = true;
			m_PlayOnce = true;
			m_Text = { "x " + std::to_string(m_AmountOfCoins) + "/7" };

			m_pMarioCharacter->GetTransform()->Translate(-28.f, 13.f, -28.f);

			for (const CoinData& data : m_CoinDates)
			{
				CoinPrefab* coin = AddChild(new CoinPrefab(data.x, data.y, data.z, data.rotationSpeed, this));
				m_pCoins.push_back(coin);
			}


		}
	}

	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_Text), m_TextPosition, m_TextColor);
	
	if (m_pMarioIconSprites.empty())
	{
		m_DelayEnd += m_SceneContext.pGameTime->GetElapsed();

		const auto& clip = pAnimator->GetClipName();
		if (clip != L"Death")
		{
			m_pChannel0->stop();
			pAnimator->SetAnimation(L"Death");
			m_pChannel2->setPaused(true);

			auto pFmod = SoundManager::Get()->GetSystem();
			pFmod->playSound(m_pSoundDeath, nullptr, false, &m_pChannel7);
			m_pChannel7->setMode(FMOD_LOOP_OFF);
			m_pChannel7->set3DMinMaxDistance(0.f, 0.f);
		}

		if(m_DelayEnd >= 2.5f)
		{
			SceneManager::Get()->NextScene();
			m_AmountOfCoins = 0;
			m_DelayEnd = 0;
			m_Text = { "x " + std::to_string(m_AmountOfCoins) + "/7" };
			m_PlayOnce = true;

			m_pMarioIconSprites.resize(3);

			for (size_t i{}; i < m_pMarioIconSprites.size(); ++i)
			{
				m_pMarioIconSprites[i] = AddChild(new GameObject());
				m_pMarioIconSprites[i]->AddComponent(new SpriteComponent(L"Textures/MarioIcon.png", { 0.5f,0.5f }));

				m_pMarioIconSprites[i]->GetTransform()->Translate(50.f + (50.f * i), 50.f, .9f);
				m_pMarioIconSprites[i]->GetTransform()->Scale(2.f, 2.f, 2.f);
			}

			m_pMarioCharacter->GetTransform()->Translate(-28.f, 13.f, -28.f);
		}
	}
	else if (m_pMarioCharacter->GetFallSpeed() > 0)
	{
		const auto& clip = pAnimator->GetClipName();
		if (clip != L"Jumping")
		{
			pAnimator->SetAnimation(L"Jumping");
			m_pChannel2->setPaused(true);

			auto pFmod = SoundManager::Get()->GetSystem();
			pFmod->playSound(m_pSoundJump, nullptr, false, &m_pChannel1);
			m_pChannel1->setMode(FMOD_LOOP_OFF);
			m_pChannel1->set3DMinMaxDistance(0.f, 0.f);
		}
	}
	else if (m_pMarioCharacter->GetFallSpeed() <= -5.f)
	{
		const auto& clip = pAnimator->GetClipName();
		if (clip != L"Falling")
		{
			pAnimator->SetAnimation(L"Falling");
			m_pChannel2->setPaused(true);
		}
	}
	else if (m_pMarioCharacter->GetMoveSpeed() > 0.f)
	{
		const auto& clip = pAnimator->GetClipName();
		if(clip != L"Running")
		{
			pAnimator->SetAnimation(L"Running");

			auto pFmod = SoundManager::Get()->GetSystem();
			pFmod->playSound(m_pSoundRun, nullptr, false, &m_pChannel2);
			m_pChannel2->setMode(FMOD_LOOP_NORMAL);
			m_pChannel2->set3DMinMaxDistance(0.f, 0.f);
		}
	}
	else if(m_pMarioCharacter->GetMoveSpeed() <= 0.f)
	{
		const auto& clip = pAnimator->GetClipName();
		if (clip != L"Idle")
		{
			pAnimator->SetAnimation(L"Idle");
			m_pChannel2->setPaused(true);
		}
	}

	//Remove Coins
	for (auto it = m_pCoins.begin(); it != m_pCoins.end();)
	{
		if ((*it)->IsRemoved())
		{
			if ((*it)->GetDoOnce())
			{
				auto pFmod = SoundManager::Get()->GetSystem();
				pFmod->playSound(m_pSoundCoinCollected, nullptr, false, &m_pChannel3);
				m_pChannel3->setMode(FMOD_LOOP_OFF);
				m_pChannel3->set3DMinMaxDistance(0.f, 0.f);
				++m_AmountOfCoins;
				m_Text = { "x " + std::to_string(m_AmountOfCoins) + "/7" };
				(*it)->SetDoOnce(false);
			}

			it = m_pCoins.erase(it);
		}
		else
		{
			++it;
		}
	}

	//Remove Goombas
	for (auto it = m_pGoombas.begin(); it != m_pGoombas.end();)
	{
		if ((*it)->IsRemoved())
		{
			if ((*it)->GetDoOnce())
			{
				auto pFmod = SoundManager::Get()->GetSystem();
				pFmod->playSound(m_pSoundGoombaDeath, nullptr, false, &m_pChannel4);
				m_pChannel4->setMode(FMOD_LOOP_OFF);
				m_pChannel4->set3DMinMaxDistance(0.f, 0.f);
				(*it)->SetDoOnce(false);
			}

			it = m_pGoombas.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'A'))
	{
		m_pMarioObject->GetTransform()->Rotate(0.f, m_pMarioCharacter->GetTotalYaw() + 90.f, 0.f);
	}

	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'D'))
	{
		m_pMarioObject->GetTransform()->Rotate(0.f, m_pMarioCharacter->GetTotalYaw() - 90.f, 0.f);
	}

	if(m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'W') && m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'A'))
	{
		m_pMarioObject->GetTransform()->Rotate(0.f, m_pMarioCharacter->GetTotalYaw() + 135.f, 0.f);
	}
	else if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'W') && m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'D'))
	{
		m_pMarioObject->GetTransform()->Rotate(0.f, m_pMarioCharacter->GetTotalYaw() - 135.f, 0.f);
	}
	else if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'W'))
	{
		m_pMarioObject->GetTransform()->Rotate(0.f, m_pMarioCharacter->GetTotalYaw() - 180.f, 0.f);
	}

	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'S') && m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'A'))
	{
		m_pMarioObject->GetTransform()->Rotate(0.f, m_pMarioCharacter->GetTotalYaw() + 45.f, 0.f);
	}
	else if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'S') && m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'D'))
	{
		m_pMarioObject->GetTransform()->Rotate(0.f, m_pMarioCharacter->GetTotalYaw() - 45.f, 0.f);
	}
	else if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'S'))
	{
		m_pMarioObject->GetTransform()->Rotate(0.f, m_pMarioCharacter->GetTotalYaw(), 0.f);
	}

	m_pMarioObject->GetTransform()->Translate(m_pMarioCharacter->GetTransform()->GetPosition().x, m_pMarioCharacter->GetTransform()->GetPosition().y - 0.9f, m_pMarioCharacter->GetTransform()->GetPosition().z);
}

void Mario64Scene::PostDraw()
{
	if (m_DrawShadowMap) 
	{
		ShadowMapRenderer::Get()->Debug_DrawDepthSRV({ m_SceneContext.windowWidth - 10.f, 10.f }, { m_ShadowMapScale, m_ShadowMapScale }, { 1.f,0.f });
	}
}

void Mario64Scene::OnGUI()
{
	m_pMarioCharacter->DrawImGui();
	ImGui::Checkbox("Draw ShadowMap", &m_DrawShadowMap);
	ImGui::SliderFloat("ShadowMap Scale", &m_ShadowMapScale, 0.f, 1.f);

	bool isEnabled = m_pPostBloom->IsEnabled();
	ImGui::Checkbox("Bloom PP", &isEnabled);
	m_pPostBloom->SetIsEnabled(isEnabled);
}

void Mario64Scene::InitMario()
{	
	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,1.f,0.f };
	settings.minSize = 0.1f;
	settings.maxSize = 0.3f;
	settings.minEnergy = 0.3f;
	settings.maxEnergy = 0.5f;
	settings.minScale = 0.1f;
	settings.maxScale = 0.3f;
	settings.minEmitterRadius = .1f;
	settings.maxEmitterRadius = .2f;
	settings.color = { 1.f,1.f,1.f, .6f };

	DiffuseMaterial_Shadow_Skinned* pMaterialMario1 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pMaterialMario1->SetDiffuseTexture(L"Textures/Mario64Body_alb.png");

	DiffuseMaterial_Shadow_Skinned* pMaterialMario2 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pMaterialMario2->SetDiffuseTexture(L"Textures/Mario64Cap_alb.png");

	DiffuseMaterial_Shadow_Skinned* pMaterialMario3 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pMaterialMario3->SetDiffuseTexture(L"Textures/Mario64Eye_alb.0.png");

	DiffuseMaterial_Shadow_Skinned* pMaterialMario4 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pMaterialMario4->SetDiffuseTexture(L"Textures/Mario64Face_alb.png");

	DiffuseMaterial_Shadow_Skinned* pMaterialMario5 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pMaterialMario5->SetDiffuseTexture(L"Textures/Mario64Hand_alb.png");

	const auto pMeshMarioModel = new ModelComponent(L"Meshes/MarioModel/mario_animation.ovm");
	pMeshMarioModel->SetMaterial(pMaterialMario2, 0);
	pMeshMarioModel->SetMaterial(pMaterialMario1, 1);
	pMeshMarioModel->SetMaterial(pMaterialMario4, 2);
	pMeshMarioModel->SetMaterial(pMaterialMario2, 3);
	pMeshMarioModel->SetMaterial(pMaterialMario3, 4);
	pMeshMarioModel->SetMaterial(pMaterialMario2, 5);
	pMeshMarioModel->SetMaterial(pMaterialMario5, 6);
	pMeshMarioModel->SetMaterial(pMaterialMario5, 7);

	m_pErmitterWalk = m_pMarioObject->AddComponent(new ParticleEmitterComponent(L"Textures/Jump.png", settings, 5));
	m_pMarioObject->AddComponent(pMeshMarioModel);
	m_pMarioObject->GetTransform()->Scale(0.2f, 0.2f, 0.2f);

	pAnimator = pMeshMarioModel->GetAnimator();
	pAnimator->SetAnimation(m_AnimationClipId);
	pAnimator->SetAnimationSpeed(m_AnimationSpeed);
}

void Mario64Scene::InitMap()
{
	//Initialize Map
	const auto pMapObject = AddChild(new GameObject());

	//Create Map Model
	const auto pMeshModel = new ModelComponent(L"Meshes/Bob-omb-Battlefield/bobomb battlefeild.ovm");
	pMapObject->AddComponent(pMeshModel);

	//Add Map RigidBody for collision
	const auto pMapRigidBody = new RigidBodyComponent(true);
	pMapObject->AddComponent(pMapRigidBody);

	//Load Triangle Mesh Map
	const auto pTriangleMeshMap = ContentManager::Load<PxTriangleMesh>(L"Meshes/Bob-omb-Battlefield/bobomb battlefeild.ovpt");
	pMapRigidBody->AddCollider(PxTriangleMeshGeometry(pTriangleMeshMap, PxMeshScale({ 1.f,1.f,1.f })), *m_pDefaultMat);
	pMapObject->GetTransform()->Scale(1.f, 1.f, 1.f);

	DiffuseMaterial_Shadow* pMaterial1 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial1->SetDiffuseTexture(L"Textures/12436720_c.bmp");

	DiffuseMaterial_Shadow* pMaterial2 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial2->SetDiffuseTexture(L"Textures/41A41EE3_c.png");

	DiffuseMaterial_Shadow* pMaterial3 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial3->SetDiffuseTexture(L"Textures/359289F2_c.bmp");

	DiffuseMaterial_Shadow* pMaterial4 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial4->SetDiffuseTexture(L"Textures/1FAAE88D_c.bmp");

	DiffuseMaterial_Shadow* pMaterial5 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial5->SetDiffuseTexture(L"Textures/6E3A21B_c.bmp");

	DiffuseMaterial_Shadow* pMaterial6 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial6->SetDiffuseTexture(L"Textures/6B1A233B_c.bmp");

	DiffuseMaterial_Shadow* pMaterial7 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial7->SetDiffuseTexture(L"Textures/6B2D96F_c.bmp");

	DiffuseMaterial_Shadow* pMaterial8 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial8->SetDiffuseTexture(L"Textures/4020CDFE_c.bmp");

	DiffuseMaterial_Shadow* pMaterial9 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial9->SetDiffuseTexture(L"Textures/275F399C_c.bmp");

	DiffuseMaterial_Shadow* pMaterial10 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial10->SetDiffuseTexture(L"Textures/4020CDFE_c.bmp");

	DiffuseMaterial_Shadow* pMaterial11 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial11->SetDiffuseTexture(L"Textures/1B46C8C_c.bmp");

	DiffuseMaterial_Shadow* pMaterial12 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial12->SetDiffuseTexture(L"Textures/6C631877_c.bmp");

	DiffuseMaterial_Shadow* pMaterial13 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial13->SetDiffuseTexture(L"Textures/3D49A9D5_c.bmp");

	DiffuseMaterial_Shadow* pMaterial14 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial14->SetDiffuseTexture(L"Textures/C1DF883_c.bmp");

	DiffuseMaterial_Shadow* pMaterial15 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial15->SetDiffuseTexture(L"Textures/3F485258_c.bmp");

	DiffuseMaterial_Shadow* pMaterial16 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial16->SetDiffuseTexture(L"Textures/10E99677_c.bmp");

	DiffuseMaterial_Shadow* pMaterial17 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pMaterial17->SetDiffuseTexture(L"Textures/574B138E_c.png");

	pMeshModel->SetMaterial(pMaterial17, 0U);
	pMeshModel->SetMaterial(pMaterial2, 1U);
	pMeshModel->SetMaterial(pMaterial4, 2U);
	pMeshModel->SetMaterial(pMaterial3, 3U);
	pMeshModel->SetMaterial(pMaterial5, 4U);
	pMeshModel->SetMaterial(pMaterial6, 5U);
	pMeshModel->SetMaterial(pMaterial7, 6U);
	pMeshModel->SetMaterial(pMaterial1, 7U);
	pMeshModel->SetMaterial(pMaterial4, 8U);
	pMeshModel->SetMaterial(pMaterial4, 9U);
	pMeshModel->SetMaterial(pMaterial9, 10U);
	pMeshModel->SetMaterial(pMaterial10, 11U);
	pMeshModel->SetMaterial(pMaterial11, 12U);
	pMeshModel->SetMaterial(pMaterial12, 13U);
	pMeshModel->SetMaterial(pMaterial13, 14U);
	pMeshModel->SetMaterial(pMaterial14, 15U);
	pMeshModel->SetMaterial(pMaterial15, 16U);
	pMeshModel->SetMaterial(pMaterial16, 17U);
	pMeshModel->SetMaterial(pMaterial15, 18U);
	pMeshModel->SetMaterial(pMaterial16, 19U);
	pMeshModel->SetMaterial(pMaterial3, 20U);
	pMeshModel->SetMaterial(pMaterial7, 21U);
	pMeshModel->SetMaterial(pMaterial1, 22U);
	pMeshModel->SetMaterial(pMaterial1, 23U);
	pMeshModel->SetMaterial(pMaterial1, 24U);
}
