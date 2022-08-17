#pragma once

// Include shader
#include "RenderControl\shader.h"
#include "GUI_Scene2D.h"
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

// Include CEnemyBase
#include "EnemyBase.h"

class CTurretBase : public CEntity2D
{
public:
	// Constructor
	CTurretBase(void);

	// Destructor
	virtual ~CTurretBase(void);

	// Init
	bool Init(int x, int y);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);
	
	// boolean flag to indicate if this enemy is active
	bool bIsActive;

	void SetEnemyList(std::vector<CEnemyBase*>* newEnemyList);
	void SetTurretLevel(int level);

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
		TARGET = 1,
		ATTACK = 2,
		NUM_FSM
	};

	glm::vec2 vec2OldIndex;

	unsigned int Ammo;
	unsigned int Level;
	int damageTimer;

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
	CGUI_Scene2D* cGUIScene2D;
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

	std::vector<CEnemyBase*>* cEnemyList;

	void Constraint(DIRECTION eDirection = LEFT);

	// Check if a position is possible to move into
	bool CheckPosition(DIRECTION eDirection);

	// Let turret interact with the enemies
	bool InteractWithEnemy(void);

	// Update direction
	void UpdateDirection(void);

	// Flip horizontal direction. For patrol use only
	void FlipHorizontalDirection(void);

	// Update position
	void UpdatePosition(void);

	
};

