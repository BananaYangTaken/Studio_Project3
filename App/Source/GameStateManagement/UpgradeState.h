#pragma once

/**
 CUpgradeState
 @brief This class is derived from CGameState. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

#include "GameStateBase.h"
#include "../Scene2D/Scene2D.h"
#include "Primitives/Mesh.h"
#include "../Scene2D/BackgroundEntity.h"

 // Include ImageLoader
#include "System\ImageLoader.h"

#include <string>
#include "GameControl/Settings.h"
#include "../SoundController/SoundController.h"
 // Include IMGUI
 // Important: GLEW and GLFW must be included before IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif

class CUpgradeState : public CGameStateBase
{
public:
	// Constructor
	CUpgradeState(void);
	// Destructor
	~CUpgradeState(void);

	// Init this class instance
	virtual bool Init(void);
	//change button stuff
	void Preload(void);
	// Update this class instance
	virtual bool Update(const double dElapsedTime);
	// Render this class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);

protected:
	struct ButtonData
	{
		std::string fileName;
		unsigned textureID;
	};

	ButtonData WindowUpgrade[3], TurretUpgrade[2], Barbwire[2];
		

	CImageLoader* il;

	CSettings* cSettings;
	CSoundController* cSoundController;

	CScene2D* cScene2D;

	int CurrentWindowLv = 0;
	int CurrentTurretLv = 0;
	int CurrentBarbWireLv = 0;


};
