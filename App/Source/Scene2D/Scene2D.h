/**
 CScene2D
 @brief A class which manages the 2D game scene
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include SingletonTemplate
#include "DesignPatterns/SingletonTemplate.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include vector
#include <vector>

// Include Keyboard controller
#include "Inputs\KeyboardController.h"]

// Include SoundController
#include "..\SoundController\SoundController.h"

// Include Map
#include "Map2D.h"

// Include Entities
#include "Player2D.h"
#include "Player2D_V2.h"
#include "Object2D.h"
#include "EnemyBase.h"
#include "Projectile2D.h"
#include "Projectile2DManager.h"

// Include GUI
#include "GUI_Scene2D.h"

// Include Game Manager
#include "GameManager.h"

// Add your include files here

class CScene2D : public CSingletonTemplate<CScene2D>
{
	friend CSingletonTemplate<CScene2D>;
public:
	// Init
	bool Init(void);

	// Update
	bool Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	//Restart Level
	void RestartLevel(void);

	//Load in Objects
	void LoadObjects(void);

	//Load in Enemies
	void LoadEnemies(void);

	void Destroy(void);

protected:
	//Handler containing the instance of 2D Map
	CMap2D* cMap2D;
	//Handler containing the instance of 2D player 1
	CPlayer2D_V2* Player1;
	//Handler containing the instance of 2D player 2
	CPlayer2D_V2* Player2;
	//Handler containing the instance of 2D Gui
	CGUI_Scene2D* cGUI_Scene2D;
	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;
	//Handler containing the instance of Game Manager
	CGameManager* cGameManager;
	//Handler to the Settings
	CSettings* cSettings;
	//Handler to the CSoundController
	CSoundController* cSoundController;
	//Handler to Projectile Manager
	CProjectileManager* cProjectileManager;

	//A vector containing instances of CEnemyBase Derived Class
	std::vector<CEnemyBase*> cEnemyList;
	//A vector containing instances of CObject2Ds
	std::vector<CObject2D*> cObjectList;

	// A transformation matrix for controlling where to render the entities
	glm::mat4 transform;

	// Add your variables and methods here.

	// Constructor
	CScene2D(void);
	// Destructor
	virtual ~CScene2D(void);
};
