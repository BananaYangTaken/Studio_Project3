/**
 CPlayer2D
 @brief A class representing the player object
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

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
#include "GUI_Scene2D.h"
#include "GameManager.h"
#include "Object2D.h"
#include "Projectile2DManager.h"
#include "Mouse2D.h"
// Include SoundController
#include "..\SoundController\SoundController.h"
//Include Physics2D
#include "Physics2D.h"

// Include Singleton template
#include "DesignPatterns\SingletonTemplate.h"

// Include Keyboard controller
#include "Inputs\KeyboardController.h"
// Include MouseController controller
#include "Inputs/MouseController.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

class CPlayer2D_V2 : public CSingletonTemplate<CPlayer2D_V2>, public CEntity2D
{
	friend CSingletonTemplate<CPlayer2D_V2>;
public:

	// Init
	bool Init(void);

	// Reset
	bool Reset(void);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	// Constructor
	CPlayer2D_V2(void);

	// Destructor
	virtual ~CPlayer2D_V2(void);

	//cObjectList Setter
	void SetObjectList(std::vector< CObject2D*>*);
	void SetEnemyList(std::vector< CEntity2D*>*);

	//LoadObject Setter and Getter
	void SetLoadObject(bool);
	bool GetLoadObject(void);

	//Getter and Setter for Health and InvulnerabilityFrames
	void SetHealth(float);
	void SetInvulnerabilityFrame(float);
	float GetHealth(void);
	float GetMaxHealth(void);
	float GetInvulnerabilityFrame(void);
	bool unlockedYellow = false;
	unsigned int GetDirection(void);

	void SetMotionFlag(const bool bMotion);
	bool GetMotionFlag(void);

	bool PlayerChangedPos(void);

protected:
	enum DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		NUM_DIRECTIONS
	};

	std::vector< CObject2D*>* cObjectList;

	std::vector< CEntity2D*>* cEnemyList;

	glm::vec2 vec2OldIndex;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;
	
	// Handler to the CGUI instance
	CGUI_Scene2D* cGUI_Scene2D;

	//Physics
	CPhysics2D cPhysics2D;

	//Handler to the Inventory Manager
	CInventoryManager* cInventoryManager;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;
	// Mouse Controller singleton instance
	CMouseController* cMouseController;
	// Projectile Handler
	CProjectileManager* cProjectileManager;
	//Item pointer
	CInventoryItem* cInventoryItem;

	//Handler containing the instance of Game Manager
	CGameManager* cGameManager;

	CSpriteAnimation* animatedSprites;

	CSoundController* cSoundController;
	
	CMouse2D* Mouse;

	// Player's colour
	glm::vec4 runtimeColour;

	float Health;
	float MaxHealth;
	float InvulnerabilityFrame;
	float Speed;

	// Player's Current Direction
	unsigned int Direction;
	bool motion;
	float AnimationTimer;
	float healtimer = 0;
	//If player interact with map and creates a new object
	bool LoadObject;

	bool LButtonState;

	// Constraint the player's position within a boundary
	void Constraint(DIRECTION eDirection = LEFT);

	//Let Player interact with map
	void InteractWithMap(int xdisplacement = 0, int ydisplacement = 0);
	
	//Check direction feasible to move in
	bool CheckPosition(DIRECTION);


};

