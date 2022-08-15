#pragma once

// Include shader
#include "RenderControl\shader.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"

// Include Settings
#include "GameControl\Settings.h"

// Include Player2D_V2
#include "Player2D_V2.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

// Include Keyboard controller
#include "Inputs/KeyboardController.h"

class CEnemyBase : public CEntity2D
{
public:
	// Constructor
	CEnemyBase(void);

	// Destructor
	~CEnemyBase(void);

	// Init
	virtual bool Init(int, int) = 0;

	// Update
	virtual void Update(const double dElapsedTime);

	// PreRender
	virtual void PreRender(void);

	// Render
	virtual void Render(void);

	// PostRender
	virtual void PostRender(void);
	
	// boolean flag to indicate if this enemy is active
	bool bIsActive;

	//Getter and Setter for Health and InvulnerabilityFrames
	virtual void SetHealth(unsigned int);
	virtual void SetInvulnerabilityFrame(float);
	virtual unsigned int GetHealth(void);
	virtual float GetInvulnerabilityFrame(void);

	virtual void SetStun(bool);

protected:
	enum DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		NUM_DIRECTIONS
	};

	enum FSM
	{
		IDLE = 0,
		PATROL = 1,
		ATTACK = 2,
		STUNNED = 3,
		RELOAD = 4,
		RETREAT = 5,
		HEAL = 6,
		EXPLOSION = 7,
		NUM_FSM
	};

	glm::vec2 vec2OldIndex;

	unsigned int Health;
	float InvulnerabilityFrame;
	bool Stunned;
	float StunDuration;
	bool Attack;
	float AttackAnim;
	float ReloadDuration;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	// A transformation matrix for controlling where to render the entities
	glm::mat4 transform;

	// The vec2 which stores the indices of the destination for enemy2D in the Map2D
	glm::vec2 vec2Destination;
	// The vec2 which stores the direction for enemy2D movement in the Map2D
	glm::vec2 vec2Direction;

	// Settings
	CSettings* cSettings;

	// Keyboard Controller 
	CKeyboardController* cKeyboardController;

	// Physics
	CPhysics2D cPhysics2D;

	// Current color
	glm::vec4 runtimeColour;

	unsigned int Direction;

	CPlayer2D_V2* Player;

	CSpriteAnimation* animatedSprites;

	//Flag for calculating A* path finding
	bool AStarCalculate;

	// Current FSM
	FSM sCurrentFSM;

	// FSM counter - count how many frames it has been in this FSM
	int iFSMCounter;

	// Max count in a state
	const int iMaxFSMCounter = 60;

	// Constraint the enemy2D's position within a boundary
	virtual void Constraint(DIRECTION eDirection = LEFT);

	// Check if a position is possible to move into
	virtual bool CheckPosition(DIRECTION eDirection);

	// Let enemy2D interact with the player
	virtual bool InteractWithPlayer(void);

	// Update direction
	virtual void UpdateDirection(void);

	// Flip horizontal direction. For patrol use only
	virtual void FlipHorizontalDirection(void);

	// Update position
	virtual void UpdatePosition(void);

	
};

