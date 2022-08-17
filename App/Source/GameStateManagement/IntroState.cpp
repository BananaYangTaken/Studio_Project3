// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "IntroState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include Mesh Builder
#include "Primitives/MeshBuilder.h"
// Include ImageLoader
#include "System\ImageLoader.h"
// Include Shader Manager
#include "RenderControl\ShaderManager.h"

 // Include shader
#include "RenderControl\shader.h"

// Include CSettings
#include "GameControl/Settings.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"
#include "../Source/System/filesystem.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CIntroState::CIntroState(void)
	: background(NULL)
{
	// Load the sounds into CSoundController
	cSoundController = CSoundController::GetInstance();
	//Loading Sounds
	{
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sound_Bell.ogg"), 1, true, false, CSoundInfo::SOUNDGROUP::Environment);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sound_Explosion.ogg"), 2, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sound_Jump.ogg"), 3, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\PlayerHit.ogg"), 4, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SkeletonHit.ogg"), 5, true, false, CSoundInfo::SOUNDGROUP::Creatures);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\ArrowHit.ogg"), 6, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\FireArrow.ogg"), 7, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Creeper.ogg"), 8, true, false, CSoundInfo::SOUNDGROUP::Creatures);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\HollowKnight-Reflection.ogg"), 9, true, true, CSoundInfo::SOUNDGROUP::Background);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\HollowKnight-Hornet.ogg"), 10, true, true, CSoundInfo::SOUNDGROUP::Background);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Terraria-BG.ogg"), 11, true, true, CSoundInfo::SOUNDGROUP::Background);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Jumping.ogg"), 12, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Landing.ogg"), 13, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\BowStretch.ogg"), 14, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SwordSwing.ogg"), 15, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Explosion.ogg"), 16, true, false, CSoundInfo::SOUNDGROUP::Creatures);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Hiss.ogg"), 17, true, false, CSoundInfo::SOUNDGROUP::Creatures);
		
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SearchingBag.ogg"), 18, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSoundSystem();
		
		
		//Add BG
		//cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sound_Jump.ogg"), 3, true, true);
	}
}

/**
 @brief Destructor
 */
CIntroState::~CIntroState(void)
{
}

/**
 @brief Init this class instance
 */
bool CIntroState::Init(void)
{
	//cout << "CIntroState::Init()\n" << endl;

	// Include Shader Manager
	CShaderManager::GetInstance()->Use("Shader2D");
	//CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	//Create Background Entity
	background = new CBackgroundEntity("Image/IntroBackground.png");
	background->SetShader("Shader2D");
	background->Init();
	cSoundController->PlaySoundByID(9);
	return true;
}

/**
 @brief Update this class instance
 */
bool CIntroState::Update(const double dElapsedTime)
{
	//cout << "CIntroState::Update()\n" << endl;
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_SPACE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		//cout << "Loading MenuState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		return true;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CIntroState::Render()
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Draw the background
 	background->Render();
}

/**
 @brief Destroy this class instance
 */
void CIntroState::Destroy(void)
{
	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}
	cSoundController->StopSoundByID(9);

	//cout << "CIntroState::Destroy()\n" << endl;
}