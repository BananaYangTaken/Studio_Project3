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
#include "Projectile2D.h"
// Include SoundController
#include "..\SoundController\SoundController.h"
//Include Physics2D
#include "Physics2D.h"

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

class CPlayer2D_V2 : public CEntity2D
{
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

	//Player Getter and Setter
	void SetPlayer(unsigned int);
	unsigned int GetPlayer(void);

	//cObjectList Setter
	void SetObjectList(std::vector< CObject2D*>*);
	//cProjectList Setter
	void SetProjectileList(std::vector< CProjectile2D*>*);

	//LoadObject Setter and Getter
	void SetLoadObject(bool);
	bool GetLoadObject(void);
	//Getter and Setter for jumpcount and teleportcount
	void SetTeleportPower(bool);
	bool GetTeleportPower(void);
	void SetJumpCount(unsigned int);
	unsigned int GetJumpCount(void);
	unsigned int GetAtExit(void);

	//Getter and Setter for Health and InvulnerabilityFrames
	void SetHealth(unsigned int);
	void SetInvulnerabilityFrame(float);
	unsigned int GetHealth(void);
	float GetInvulnerabilityFrame(void);

	bool Getattack(void);
	unsigned int GetDirection(void);

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
	std::vector< CProjectile2D*>* cProjectileList;

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

	//Item pointer
	CInventoryItem* cInventoryItem;

	//Handler containing the instance of Game Manager
	CGameManager* cGameManager;

	CSpriteAnimation* animatedSprites;

	CSoundController* cSoundController;

	// Player's colour
	glm::vec4 runtimeColour;

	unsigned int Health;
	float InvulnerabilityFrame;

	// Player's Current Direction
	unsigned int Direction;
	bool idle;
	bool attack;
	float attackAnim;

	// Player interactions
	bool OnLadder;
	unsigned int jumpcount;  //Number of Times player can jump
	bool teleportpower; //teleport
	float jumpheightmodifier;  //How powerful the players 1st jump is
	float speedmodifier; //How fast the player can go
	float Timer;   //modifier Timer
	bool PhaseThroughWall;
	float PhaseTimer; //Phasethroughwall Timer
	float DoubleTapTimer;
	unsigned int TapCount;
	unsigned int PreviousTapDirection;
	unsigned int CurrentTapDirection;
	bool AtExit;

	//If before restart already own those amount of jumps dont change
	unsigned int Prejumpcount;


	//Amount of times player jumped
	int iJumpCount = 0;

	//If player interact with map and creates a new object
	bool LoadObject;

	// Constraint the player's position within a boundary
	void Constraint(DIRECTION eDirection = LEFT);

	//Let Player interact with map
	void InteractWithMap(int , int);
	
	//Check direction feasible to move in
	bool CheckPosition(DIRECTION);

	//Teleport to suitable position (move 5 blocks in x-axis in either direction depending on available space to move)
	void Teleport(DIRECTION);

	//Update Jump Fall
	void UpdateJumpFall(const double dElaspedTime);
	
	//Check if player is in mid-air
	bool IsMidAir(void);

private:
	unsigned int Player;
};

