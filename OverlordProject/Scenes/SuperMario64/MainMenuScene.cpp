#include "stdafx.h"
#include "MainMenuScene.h"

void MainMenuScene::Initialize()
{
	//Setting 
	//----------------------------------------------
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;

	m_pMainMenuSprite = AddChild(new GameObject());
	m_pMainMenuSprite->AddComponent(new SpriteComponent(L"Textures/MainMenu.png", { 0.f,0.f }));

	m_SceneContext.pInput->CursorVisible(true);
}

void MainMenuScene::Update()
{
	if (InputManager::IsMouseButton(InputState::released, VK_LBUTTON))
	{
		const POINT& mousePosition = m_SceneContext.pInput->GetMousePosition();
		const XMFLOAT2& playButtonPos = { m_SceneContext.windowWidth / 2.f - 250.f, m_SceneContext.windowHeight / 2.f + 100.f };

		// Calculate the distance between the mouse position and the scene center
		float dx = static_cast<float>(mousePosition.x) - playButtonPos.x;
		float dy = static_cast<float>(mousePosition.y) - playButtonPos.y;
		float distanceSquared = dx * dx + dy * dy;

		// Define a threshold distance squared for considering it a click in the middle
		float clickThresholdSquared = 90.0f * 90.0f;

		// Check if the distance squared is within the click threshold squared
		if (distanceSquared <= clickThresholdSquared)
		{
			SceneManager::Get()->NextScene();
		}
	}
}
