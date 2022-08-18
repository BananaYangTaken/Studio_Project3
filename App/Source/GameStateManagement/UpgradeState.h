#pragma once

/**
 CUpgradeState
 @brief This class is derived from CGameState. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

#include "GameStateBase.h"
#include "../App/Source/Scene2D/GUI_Scene2D.h"
#include "../Scene2D/Scene2D.h"
#include "../App/Source/Scene2D/InventoryManager.h"
#include "Primitives/Mesh.h"
#include "../Scene2D/BackgroundEntity.h"
#include "../Scene2D/Map2D.h"

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
	std::string itemlist[3] = { "empty1", "empty2", "empty3" };
	int itemlistcount[3];
	bool SearchForRequirements( std::string list[3], int listcount[3]);
	std::string Turretitemlist[3] = { "empty1", "empty2", "empty3" };
	int Turretitemlistcount[3];

	std::string Wireitemlist[3] = { "empty1", "empty2", "empty3"};
	int Wireitemlistcount[3];
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

	ButtonData WindowUpgrade[3], TurretUpgrade[3], Barbwire[2];
	
	
	CGUI_Scene2D* cGUIScene2D;
	CImageLoader* il;
	CInventoryItem* cInventoryItem;
	CInventoryManager* cInventoryManager;
	CSettings* cSettings;
	CSoundController* cSoundController;

	CScene2D* cScene2D;
	CMap2D* cMap2D;

	int CurrentWindowLv = 0;
	int CurrentTurretLv = 0;
	int CurrentBarbWireLv = 0;



};

