// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

#include "PlayGameState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"
#include "../Scene2D/GameManager.h"
#include "../SoundController/SoundController.h"
#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CPlayGameState::CPlayGameState(void)
	: cScene2D(NULL)
{

}

/**
 @brief Destructor
 */
CPlayGameState::~CPlayGameState(void)
{

}

/**
 @brief Init this class instance
 */
bool CPlayGameState::Init(void)
{
	//cout << "CPlayGameState::Init()\n" << endl;

	// Initialise the cScene2D instance
	cScene2D = CScene2D::GetInstance();
	if (cScene2D->Init() == false)
	{
		cout << "Failed to load Scene2D" << endl;
		return false;
	}
	//CSoundController::GetInstance()->PlaySoundByID(11);

	return true;
}

/**
 @brief Update this class instance
 */
bool CPlayGameState::Update(const double dElapsedTime)
{
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		//cout << "Loading MenuState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		CGameStateManager::GetInstance()->OffPauseGameState();
		return true;
	}
	else if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_F10))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		//cout << "Loading PauseState" << endl;
		CGameStateManager::GetInstance()->SetPauseGameState("PauseState");
	}
	else if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_F6))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		//load the upgrade state
		CGameStateManager::GetInstance()->SetUpgradeState("UpgradeState");
	}
	if (CGameManager::GetInstance()->bPlayerLost)
	{
		CGameStateManager::GetInstance()->SetActiveGameState("LoseState");
		CGameStateManager::GetInstance()->OffPauseGameState();
		CGameManager::GetInstance()->bPlayerLost = false;
	}
	else if (CGameManager::GetInstance()->bPlayerWon)
	{
		CGameStateManager::GetInstance()->SetActiveGameState("WinState");
		CGameStateManager::GetInstance()->OffPauseGameState();
		CGameManager::GetInstance()->bPlayerWon = false;
	}

	// Call the cScene2D's Update method
	cScene2D->Update(dElapsedTime);

	return true;
}

/**
 @brief Render this class instance
 */
void CPlayGameState::Render(void)
{
	//cout << "CPlayGameState::Render()\n" << endl;

	// Call the cScene2D's Pre-Render method
	cScene2D->PreRender();

	// Call the cScene2D's Render method
	cScene2D->Render();

	// Call the cScene2D's PostRender method
	cScene2D->PostRender();
}

/**
 @brief Destroy this class instance
 */
void CPlayGameState::Destroy(void)
{
	//cout << "CPlayGameState::Destroy()\n" << endl;

	// Destroy the cScene2D instance
	if (cScene2D)
	{
		cScene2D->Destroy();
		cScene2D = NULL;
	}
	CSoundController::GetInstance()->StopSoundByID(11);
}