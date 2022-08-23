#include "TurretBase.h"

#include <iostream>

// Include Shader Manager
#include "RenderControl\ShaderManager.h"
// Include Mesh Builder
#include "Primitives/MeshBuilder.h"

// Include GLEW
#include <GL/glew.h>

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
// Include math.h
#include <math.h>

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CTurretBase::CTurretBase(void)
{
	bIsActive = false;
	cMap2D = NULL;
	cSettings = NULL;
	Player = NULL;
	sCurrentFSM = IDLE;
	iFSMCounter = 0;
	animatedSprites = NULL;
	AStarCalculate = true;
	Level = 0;
	damageTimer = 0;
	changedBase = 0;

	transform = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::vec2(0);

	// Initialise vecNumMicroSteps
	vec2NumMicroSteps = glm::vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	vec2Destination = glm::vec2(0, 0);	// Initialise the iDestination
	vec2Direction = glm::vec2(0, 0);		// Initialise the iDirection

	Player = CPlayer2D_V2::GetInstance();
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CTurretBase::~CTurretBase(void)
{
	if (animatedSprites)
	{
		delete animatedSprites;
		animatedSprites = NULL;
	}

	// We won't delete this since it was created elsewhere
	Player = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/**
  @brief Initialise this instance
  */
bool CTurretBase::Init(int x, int y)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();
	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	cGUIScene2D = CGUI_Scene2D::GetInstance();
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(401, uiRow, uiCol) == false)
	{
		return false;	// Unable to find the start position of the enemy, so quit this game
	}
	// Erase the value of the enemy in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the enemy to iRow and iCol
	vec2Index = glm::vec2(x, y);

	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/TurretSprites.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/TurretSprites.png" << endl;
		return false;
	}

	//CS:Create Animated Sprites and setup animation


	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 9, (cSettings->TILE_WIDTH + cSettings->TILE_HEIGHT) * 1.5, (cSettings->TILE_WIDTH + cSettings->TILE_HEIGHT) * 1.5);
	animatedSprites->AddAnimation("L1idle", 0, 0);
	animatedSprites->AddAnimation("L1Attack", 0, 2);
	animatedSprites->AddAnimation("L2idle", 3, 3);
	animatedSprites->AddAnimation("L2Attack", 3, 5);
	animatedSprites->AddAnimation("L3idle", 6, 6);
	animatedSprites->AddAnimation("L3Attack", 6, 8);
	runtimeColour = glm::vec4(1, 1, 1, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	Rotation = 0;

	UpdateDirection();

	return true;
}

void CTurretBase::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;

	if (damageTimer > 0)
	{
		damageTimer -= dElapsedTime;
	}
	if (Level == 0 && changedBase == 0)
	{
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 116);
		changedBase++;
	}
	else if (Level == 1 && changedBase == 1)
	{
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 117);
		changedBase++;
	}
	else if (Level == 2 && changedBase == 2)
	{
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 118);
		changedBase++;
	}
	switch (sCurrentFSM)
	{
	case IDLE:
		//FSM Transition
		for (int i = 0; i < cEnemyList->size(); i++)
		{
			if (cPhysics2D.CalculateDistance(vec2Index, dynamic_cast<CEnemyBase*>((*cEnemyList)[i])->vec2Index) < 20.0f)
			{
				sCurrentFSM = TARGET;
				iFSMCounter = 0;
			}
		}
		if (iFSMCounter > iMaxFSMCounter)
		{
			sCurrentFSM = TARGET;
			iFSMCounter = 0;
		}
		iFSMCounter++;
		//Animation
		switch (Level)
		{
		case 0:
			animatedSprites->PlayAnimation("L1idle", -1, 1.0f);
			break;
		case 1:
			animatedSprites->PlayAnimation("L2idle", -1, 1.0f);
			break;
		case 2:
			animatedSprites->PlayAnimation("L3idle", -1, 1.0f);
			break;
		}
		
		break;
	case TARGET:
		//FSM Transition
		for (int i = 0; i < cEnemyList->size(); i++)
		{
			if (cPhysics2D.CalculateDistance(vec2Index, dynamic_cast<CEnemyBase*>((*cEnemyList)[i])->vec2Index) < 10.0f)
			{
				sCurrentFSM = ATTACK;
				iFSMCounter = 0;
			}
		}
		if (iFSMCounter > iMaxFSMCounter)
		{
			sCurrentFSM = IDLE;
			iFSMCounter = 0;
		}
		iFSMCounter++;

		break;
	case ATTACK:
		//FSM Transition
		for (int i = 0; i < cEnemyList->size(); i++)
		{
			if (cPhysics2D.CalculateDistance(vec2Index, dynamic_cast<CEnemyBase*>((*cEnemyList)[i])->vec2Index) < 10.0f 
				&& dynamic_cast<CEnemyBase*>((*cEnemyList)[i])->bIsActive == true
				&& damageTimer <= 0)
			{
				// Attack
				switch (Level)
				{
				case 0:
					animatedSprites->PlayAnimation("L1Attack", 1, 0.7f);
					dynamic_cast<CEnemyBase*>((*cEnemyList)[i])->SetHealth(dynamic_cast<CEnemyBase*>((*cEnemyList)[i])->GetHealth() - 10);
					damageTimer = 1;
					CSoundController::GetInstance()->PlaySoundByID(27);

					break;
				case 1:
					animatedSprites->PlayAnimation("L2Attack", 1, 0.7f);
					dynamic_cast<CEnemyBase*>((*cEnemyList)[i])->SetHealth(dynamic_cast<CEnemyBase*>((*cEnemyList)[i])->GetHealth() - 10);
					damageTimer = 0.5;
					CSoundController::GetInstance()->PlaySoundByID(29);

					break;
				case 2:
					animatedSprites->PlayAnimation("L3Attack", 1, 0.3f);
					for (int j = 0; j < cEnemyList->size(); j++)
					{
						if (cPhysics2D.CalculateDistance(dynamic_cast<CEnemyBase*>((*cEnemyList)[i])->vec2Index, dynamic_cast<CEnemyBase*>((*cEnemyList)[j])->vec2Index) < 2.0f
							&& dynamic_cast<CEnemyBase*>((*cEnemyList)[j])->bIsActive == true
							&& dynamic_cast<CEnemyBase*>((*cEnemyList)[j]) != (*cEnemyList)[i])
						{
							dynamic_cast<CEnemyBase*>((*cEnemyList)[j])->SetHealth(dynamic_cast<CEnemyBase*>((*cEnemyList)[j])->GetHealth() - 40);
						}
					}
					dynamic_cast<CEnemyBase*>((*cEnemyList)[i])->SetHealth(dynamic_cast<CEnemyBase*>((*cEnemyList)[i])->GetHealth() - 40);
					damageTimer = 0.5;
					CSoundController::GetInstance()->PlaySoundByID(31);

					break;
				}

				//Calculate Rotation
				//Rotates based on last fired target
				Rotation = cPhysics2D.CalculateRotation(vec2Index, vec2Index + glm::vec2(1, 0), dynamic_cast<CEnemyBase*>((*cEnemyList)[i])->vec2Index);
				std::cout << "BAMMMMMMMMMMM		" << Rotation << "		BAMMMMMMMMM" << std::endl;
			}
		}

		if (iFSMCounter > iMaxFSMCounter)
		{
			sCurrentFSM = IDLE;
			iFSMCounter = 0;
		}
		iFSMCounter++;

		break;
	default:
		break;
	}

	++iFSMCounter;
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	{
		//if within viewing distance
		if ((abs(Player->vec2Index.x - vec2Index.x) <= cSettings->VIEW_TILES_XAXIS * 0.5 + 1) && (abs(Player->vec2Index.y - vec2Index.y) <= cSettings->VIEW_TILES_YAXIS * 0.5 + 1))
		{
			//Calculate Position of Entity on Screen
			glm::vec2 ScreenPos = glm::vec2(0, 0);
			glm::vec2 TotalMicroStep = vec2NumMicroSteps;
			//Check if Map View/Camera at Borders
			if (Player->vec2Index.x < cSettings->VIEW_TILES_XAXIS * 0.5) // Left Side Border
			{
				ScreenPos.x = vec2Index.x + 1;
			}
			else if (Player->vec2Index.x > (cSettings->NUM_TILES_XAXIS - (cSettings->VIEW_TILES_XAXIS * 0.5))) //Right Side Border
			{
				ScreenPos.x = cSettings->VIEW_TILES_XAXIS - (cSettings->NUM_TILES_XAXIS - vec2Index.x) + 1;
			}
			else
			{
				TotalMicroStep.x = TotalMicroStep.x - Player->vec2NumMicroSteps.x;
			}

			if (Player->vec2Index.y > (cSettings->NUM_TILES_YAXIS - (cSettings->VIEW_TILES_YAXIS * 0.5))) //Top Side Border
			{
				ScreenPos.y = cSettings->VIEW_TILES_YAXIS - (cSettings->NUM_TILES_YAXIS - vec2Index.y) + 1;
			}
			else if (Player->vec2Index.y < cSettings->VIEW_TILES_YAXIS * 0.5) //Bottom Side Border
			{
				ScreenPos.y = vec2Index.y + 1;
			}
			else
			{
				TotalMicroStep.y = TotalMicroStep.y - Player->vec2NumMicroSteps.y;
			}


			//If not at Border, Entity at Center of Screen displaced by x:1 y:1
			if (ScreenPos.x == 0)
			{
				ScreenPos.x = vec2Index.x + 1 - Player->vec2Index.x + cSettings->VIEW_TILES_XAXIS * 0.5;
			}
			if (ScreenPos.y == 0)
			{
				ScreenPos.y = vec2Index.y + 1 - Player->vec2Index.y + cSettings->VIEW_TILES_YAXIS * 0.5;
			}

			//Convert position to UV Coords
			vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, ScreenPos.x - 1, false, (TotalMicroStep.x) * cSettings->MICRO_STEP_XAXIS);
			vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, ScreenPos.y - 1, false, (TotalMicroStep.y) * cSettings->MICRO_STEP_YAXIS);
		}
		else
		{
			vec2UVCoordinate = glm::vec2(2, 2);
		}
	}
}


/**
 @brief Constraint the enemy2D's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CTurretBase::Constraint(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if (vec2Index.x < 0)
		{
			vec2Index.x = 0;
			vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == RIGHT)
	{
		if (vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS) - 1;
			vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == UP)
	{
		if (vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS) - 1;
			vec2NumMicroSteps.y = 0;
		}
	}
	else if (eDirection == DOWN)
	{
		if (vec2Index.y < 0)
		{
			vec2Index.y = 0;
			vec2NumMicroSteps.y = 0;
		}
	}
	else
	{
		cout << "CEnemy2D::Constraint: Unknown direction." << endl;
	}
}

/**
 @brief Check if a position is possible to move into
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
bool CTurretBase::CheckPosition(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		// If the new position is fully within a row, then check this row only
		if (vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == RIGHT)
	{
		// If the new position is at the top row, then return true
		if (vec2Index.x >= cSettings->NUM_TILES_XAXIS - 1)
		{
			vec2NumMicroSteps.x = 0;
			return true;
		}

		// If the new position is fully within a row, then check this row only
		if (vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}

	}
	else if (eDirection == UP)
	{
		// If the new position is at the top row, then return true
		if (vec2Index.y >= cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2NumMicroSteps.y = 0;
			return true;
		}

		// If the new position is fully within a column, then check this column only
		if (vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == DOWN)
	{
		// If the new position is fully within a column, then check this column only
		if (vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else
	{
		cout << "CEnemy2D::CheckPosition: Unknown direction." << endl;
	}

	return true;
}

/**
 @brief Let enemy2D interact with the player.
 */
bool CTurretBase::InteractWithEnemy(void)
{
	glm::vec2 i32vec2PlayerPos = Player->vec2Index;

	// Check if the enemy2D is within 1.5 indices of the player2D
	if (((vec2Index.x >= i32vec2PlayerPos.x - 0.5) &&
		(vec2Index.x <= i32vec2PlayerPos.x + 0.5))
		&&
		((vec2Index.y >= i32vec2PlayerPos.y - 0.5) &&
			(vec2Index.y <= i32vec2PlayerPos.y + 0.5)))
	{
		// Since the player has been caught, then reset the FSM
		sCurrentFSM = IDLE;
		iFSMCounter = 0;
		return true;
	}
	return false;
}

/**
 @brief Update the enemy's direction.
 */
void CTurretBase::UpdateDirection(void)
{
	// Set the destination to the player
	vec2Destination = Player->vec2Index;

	// Calculate the direction between enemy2D and player2D
	vec2Direction = vec2Destination - vec2Index;

	// Calculate the distance between enemy2D and player2D
	float fDistance = cPhysics2D.CalculateDistance(vec2Index, vec2Destination);
	if (fDistance >= 0.01f)
	{
		// Calculate direction vector.
		// We need to round the numbers as it is easier to work with whole numbers for movements
		vec2Direction.x = (int)round(vec2Direction.x / fDistance);
		vec2Direction.y = (int)round(vec2Direction.y / fDistance);
	}
	else
	{
		// Since we are not going anywhere, set this to 0.
		vec2Direction = glm::vec2(0);
	}
}

/**
 @brief Flip horizontal direction. For patrol use only
 */
void CTurretBase::FlipHorizontalDirection(void)
{
	vec2Direction.x *= -1;
}

/**
@brief Update position.
*/
void CTurretBase::UpdatePosition(void)
{
	// Store the old position
	vec2OldIndex = vec2Index;

	// if the player is to the left or right of the enemy2D
	if (vec2Direction.x < 0)
	{
		// Move left
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x >= 0)
		{
			vec2NumMicroSteps.x--;
			if (vec2NumMicroSteps.x < 0)
			{
				vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
				vec2Index.x--;
			}
		}
		Direction = LEFT;

		// Constraint the enemy2D's position within the screen boundary
		Constraint(LEFT);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(LEFT) == false)
		{
			FlipHorizontalDirection();
			vec2Index.x = vec2OldIndex.x;
			vec2NumMicroSteps.x = 0;
		}


		// Interact with the Player
		InteractWithEnemy();
	}
	else if (vec2Direction.x > 0)
	{
		// Move right
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			vec2NumMicroSteps.x++;

			if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
			{
				vec2NumMicroSteps.x = 0;
				vec2Index.x++;
			}
		}
		Direction = RIGHT;

		// Constraint the enemy2D's position within the screen boundary
		Constraint(RIGHT);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(RIGHT) == false)
		{
			FlipHorizontalDirection();
			//vec2Index = vec2OldIndex;
			vec2NumMicroSteps.x = 0;
		}


		// Interact with the Player
		InteractWithEnemy();
	}

	// if the player is to the above or below of the enemy2D
	if (vec2Direction.y < 0)
	{
		// Move Down
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y--;
			if (vec2NumMicroSteps.y < 0)
			{
				vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				vec2Index.y--;
			}
		}
		Direction = DOWN;

		// Constraint the enemy2D's position within the screen boundary
		Constraint(DOWN);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(DOWN) == false)
		{
			vec2Index.y = vec2OldIndex.y;
			vec2NumMicroSteps.y = 0;
		}
		// Interact with the Player
		InteractWithEnemy();
	}
	else if (vec2Direction.y > 0)
	{
		// Move Up
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y++;

			if (vec2NumMicroSteps.y >= cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				vec2NumMicroSteps.y = 0;
				vec2Index.y++;
			}
		}
		Direction = UP;

		// Constraint the enemy2D's position within the screen boundary
		Constraint(UP);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(UP) == false)
		{
			vec2NumMicroSteps.y = 0;
		}

		// Interact with the Player
		InteractWithEnemy();
	}
}


/**
 @brief Set up the OpenGL display environment before rendering
 */
void CTurretBase::PreRender(void)
{
	if (!bIsActive)
		return;

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CTurretBase::Render(void)
{
	if (!bIsActive)
		return;

	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtimeColour");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x,
		vec2UVCoordinate.y,
		0.0f));
	transform = glm::rotate(transform, Rotation, glm::vec3(0, 0, 1));
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(runtimeColour));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);


	//Render the Player sprite
	glBindVertexArray(VAO);
	//quadMesh->Render();
	//CS: Render the animated sprites
	animatedSprites->Render();
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CTurretBase::PostRender(void)
{
	if (!bIsActive)
		return;

	// Disable blending
	glDisable(GL_BLEND);
}

void CTurretBase::SetEnemyList(std::vector<CEntity2D*>* newEnemyList)
{
	cEnemyList = newEnemyList;
}

void CTurretBase::SetTurretLevel(int newLevel)
{
	Level = newLevel;
}
