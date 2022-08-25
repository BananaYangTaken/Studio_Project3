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
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Creeper.ogg"), 8, true, false, CSoundInfo::SOUNDGROUP::Creatures);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\HollowKnight-Reflection.ogg"), 9, true, true, CSoundInfo::SOUNDGROUP::Background);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\HollowKnight-Hornet.ogg"), 10, true, true, CSoundInfo::SOUNDGROUP::Background);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Dead-Center.ogg"), 11, true, true, CSoundInfo::SOUNDGROUP::Background);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Jumping.ogg"), 12, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Landing.ogg"), 13, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\BowStretch.ogg"), 14, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SwordSwing.ogg"), 15, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Explosion.ogg"), 16, true, false, CSoundInfo::SOUNDGROUP::Creatures);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\Hiss.ogg"), 17, true, false, CSoundInfo::SOUNDGROUP::Creatures);	
		//search
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Search.ogg"), 18, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Rifle Fire
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\AK_Fire.ogg"), 19, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Craft
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Craft.ogg"), 20, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Heal
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Heal.ogg"), 21, true, false, CSoundInfo::SOUNDGROUP::Player);
		//PickUp
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\PickUp.ogg"), 22, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Pistol Fire
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Pistol_Fire.ogg"), 23, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Pistol Reload
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Pistol_Reload.ogg"), 24, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Rifle Reload
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Rifle_Reload.ogg"), 25, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Turret Active
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Turret_Active.ogg"), 26, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Turret Fire
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Turret_Fire.ogg"), 27, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Turret Idol
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Turret_Idol.ogg"), 28, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Turret2 Fire
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Turret2_Fire.ogg"), 29, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Turret3 Explosion
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Turret3_Explosion.ogg"), 30, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Turret3 Fire
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Turret3_Fire.ogg"), 31, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Zombie Attack
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Zombie_Attack2.ogg"), 32, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Zombie Chase
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Zombie_Chase2.ogg"), 33, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Zombie_Idle
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Zombie_Idle2.ogg"), 35, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Footsteps
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\FootSteps.ogg"), 36, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Keycard Accept
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\KeycardActivate.ogg"), 37, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Keycard Reject
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\KeycardReject.ogg"), 38, true, false, CSoundInfo::SOUNDGROUP::Player);
		//Horde
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\Horde.ogg"), 39, true, false, CSoundInfo::SOUNDGROUP::Player);
		cSoundController->LoadSound(FileSystem::getPath("Sounds\\SP_Sounds\\The-Horde.ogg"), 40, true, false, CSoundInfo::SOUNDGROUP::Player);
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
	background = new CBackgroundEntity("Image/IntroBackground.jpg");
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