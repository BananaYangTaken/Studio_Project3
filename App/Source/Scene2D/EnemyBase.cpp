
#include "EnemyBase.h"

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
CEnemyBase::CEnemyBase(void)
{
	

}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CEnemyBase::~CEnemyBase(void)
{

}


/**
 @brief Update this instance
 */
void CEnemyBase::Update(const double dElapsedTime)
{
	//if (!bIsActive)
	//	return;

	///*if (PlayerCloser() == Player1)
	//	std::cout << "Player1" << std::endl;
	//else
	//	std::cout << "Player2" << std::endl;*/
	//switch (sCurrentFSM)
	//{
	//case IDLE:
	//	if (iFSMCounter > iMaxFSMCounter)
	//	{
	//		sCurrentFSM = PATROL;
	//		iFSMCounter = 0;
	//		//cout << "Switching to Patrol State" << endl;
	//	}
	//	iFSMCounter++;
	//	break;
	//case PATROL:
	//	cout << "patrol" << endl;
	//	if (iFSMCounter > iMaxFSMCounter)
	//	{
	//		sCurrentFSM = IDLE;
	//		iFSMCounter = 0;
	//		//cout << "Switching to Idle State" << endl;
	//	}
	//	else if (cPhysics2D.CalculateDistance(vec2Index, Player->vec2Index) < 5.0f)
	//	{
	//		sCurrentFSM = ATTACK;
	//		iFSMCounter = 0;
	//	}
	//	else
	//	{
	//		// Patrol around
	//		// Update the Enemy2D's position for patrol
	//		UpdatePosition();
	//	}
	//	iFSMCounter++;
	//	break;
	//case ATTACK:
	//	cout << "attack" <<  endl;
	//	if (cPhysics2D.CalculateDistance(vec2Index, Player->vec2Index) < 5.0f)
	//	{
	//		// Attack
	//		// Update direction to move towards for attack
	//		//Calculate a path to the player
	//		//cMap2D->PrintSelf();
	//		//std::cout << "StartPos: " << vec2Index.x << ", " << vec2Index.y << std::endl;
	//		//std::cout << "StartPos: " << DecideTarget()->vec2Index.x << ", " << DecideTarget()->vec2Index.y << std::endl;
	//		if (AStarCalculate == true)
	//		{
	//			auto path = cMap2D->PathFind(vec2Index, Player->vec2Index, heuristic::euclidean, 10);
	//			//Calculate New Destination
	//			bool bFirstPosition = true;
	//			for (const auto& coord : path)
	//			{
	//				//std::cout << coord.x << "," << coord.y << "\n";
	//				if (bFirstPosition == true)
	//				{
	//					//Set a destination
	//					vec2Destination = coord;
	//					//Calculate the direction between enemy2D and this destination
	//					vec2Direction = vec2Destination - vec2Index;
	//					bFirstPosition = false;
	//				}
	//				else
	//				{
	//					if ((coord - vec2Destination) == vec2Direction)
	//					{
	//						//Set a destination
	//						vec2Destination = coord;
	//					}
	//					else
	//						break;
	//				}
	//			}
	//		}
	//		else if (cKeyboardController->IsKeyDown(GLFW_KEY_A) ||
	//				cKeyboardController->IsKeyDown(GLFW_KEY_S) ||
	//				cKeyboardController->IsKeyDown(GLFW_KEY_D) || 
	//				cKeyboardController->IsKeyDown(GLFW_KEY_W) || 
	//				cKeyboardController->IsKeyDown(GLFW_KEY_LEFT) || 
	//				cKeyboardController->IsKeyDown(GLFW_KEY_RIGHT) || 
	//				cKeyboardController->IsKeyDown(GLFW_KEY_UP) || 
	//				cKeyboardController->IsKeyDown(GLFW_KEY_DOWN) )
	//		{
	//			AStarCalculate = false;
	//		}
	//		//std::cout << "--- Printing out the path ---" << std::endl;
	//		// Update the Enemy2D's position for attack
	//		UpdatePosition();
	//		//cout << "i32vec2Destination : " << i32vec2Destination.x 
	//		//		<< ", " << i32vec2Destination.y << endl;
	//		//cout << "i32vec2Direction : " << i32vec2Direction.x 
	//		//		<< ", " << i32vec2Direction.y << endl;
	//		//system("pause");
	//	}
	//	else
	//	{
	//		if (iFSMCounter > iMaxFSMCounter)
	//		{
	//			sCurrentFSM = PATROL;
	//			iFSMCounter = 0;
	//			//cout << "ATTACK : Reset counter: " << iFSMCounter << endl;
	//		}
	//		iFSMCounter++;
	//	}
	//	break;
	//default:
	//	break;
	//}

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);

	if (Direction == LEFT)
	{
		animatedSprites->PlayAnimation("left", -1, 1.0f);
	}
	else if (Direction == RIGHT)
	{
		animatedSprites->PlayAnimation("right", -1, 1.0f);
	}
	if (Direction == -1)
	{
		animatedSprites->PlayAnimation("idle", -1, 1.0f);
	}
	animatedSprites->Update(dElapsedTime);


	
		
}


void CEnemyBase::SetHealth(unsigned int Health)
{
	this->Health = Health;
}

void CEnemyBase::SetDeathState(unsigned int Health)
{
	this->Health = Health;
}

void CEnemyBase::SetInvulnerabilityFrame(float InvulnerabilityFrame)
{
	this->InvulnerabilityFrame = InvulnerabilityFrame;
}

unsigned int CEnemyBase::GetHealth(void)
{
	return this->Health;
}

unsigned int CEnemyBase::GetDeathState(void)
{
	return this->Death;
}

float CEnemyBase::GetInvulnerabilityFrame(void)
{
	return this->InvulnerabilityFrame;
}

void CEnemyBase::SetPlayer(CEntity2D* newPlayer)
{
	Player = newPlayer;
}

void CEnemyBase::SetStun(bool stun)
{
	Stunned = stun;
}

void CEnemyBase::SetWindowLevel(unsigned int newLevel)
{
	Level = newLevel;
}



/**
 @brief Constraint the enemy2D's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CEnemyBase::Constraint(DIRECTION eDirection)
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
bool CEnemyBase::CheckPosition(DIRECTION eDirection)
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
bool CEnemyBase::InteractWithPlayer(void)
{
	glm::vec2 i32vec2PlayerPos = Player->vec2Index;
	
	// Check if the enemy2D is within 1.5 indices of the player2D
	if (((vec2Index.x >= i32vec2PlayerPos.x - 0.5) && 
		(vec2Index.x <= i32vec2PlayerPos.x + 0.5))
		&& 
		((vec2Index.y >= i32vec2PlayerPos.y - 0.5) &&
		(vec2Index.y <= i32vec2PlayerPos.y + 0.5)))
	{
		cout << "Gotcha!" << endl;
		// Since the player has been caught, then reset the FSM
		iFSMCounter = 0;
		return true;
	}
	return false;
}

/**
 @brief Update the enemy's direction.
 */
void CEnemyBase::UpdateDirection(void)
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
		vec2Direction.x = (int)round(vec2Direction.x);
		vec2Direction.y = (int)round(vec2Direction.y);
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
void CEnemyBase::FlipHorizontalDirection(void)
{
	vec2Direction.x *= -1;
}

/**
@brief Update position.
*/
void CEnemyBase::UpdatePosition(float speedofEnemy)
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
			vec2NumMicroSteps.x -= (1 * speedofEnemy);
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

	}
	else if (vec2Direction.x > 0)
	{
		// Move right
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			vec2NumMicroSteps.x += (1 * speedofEnemy);

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

	}

	// if the player is to the above or below of the enemy2D
	if (vec2Direction.y < 0)
	{
		// Move Down
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y -= (1 * speedofEnemy);
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

	}
	else if (vec2Direction.y > 0)
	{
		// Move Up
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y += (1 * speedofEnemy);

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

	}
}

void CEnemyBase::UpdatePositionSlowed(float speedofEnemy)
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
			vec2NumMicroSteps.x -= (0.5 * speedofEnemy);
			if (vec2NumMicroSteps.x < 0)
			{
				vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 0.5;
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

	}
	else if (vec2Direction.x > 0)
	{
		// Move right
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			vec2NumMicroSteps.x += (0.5 * speedofEnemy);

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

	}

	// if the player is to the above or below of the enemy2D
	if (vec2Direction.y < 0)
	{
		// Move Down
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y -= (0.5 * speedofEnemy);
			if (vec2NumMicroSteps.y < 0)
			{
				vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 0.5;
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

	}
	else if (vec2Direction.y > 0)
	{
		// Move Up
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y += (0.5 * speedofEnemy);

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

	}
}

void CEnemyBase::randomDir()
{
	dir = Math::RandIntMinMax(1, 4);
}

void CEnemyBase::randomiseDirectionMove(float speedofEnemy)
{
	// Store the old position
	vec2OldIndex = vec2Index;
	// if the player is to the left or right of the enemy2D
	if (dir == 1)
	{
		// Move left
		vec2NumMicroSteps.x -= (1 * speedofEnemy);
		if (vec2NumMicroSteps.x < 0)
		{
			vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
			vec2Index.x--;
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
	}
	else if (dir == 2)
	{
		// Move right
		vec2NumMicroSteps.x += (1 * speedofEnemy);
		if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
		{
			vec2NumMicroSteps.x = 0;
			vec2Index.x++;
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
	}
	else if (dir == 3)
	{
		// Move Down
		vec2NumMicroSteps.y -= (1 * speedofEnemy);
		if (vec2NumMicroSteps.y < 0)
		{
			vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
			vec2Index.y--;
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
	}
	else if (dir == 4)
	{
		// Move Up
		vec2NumMicroSteps.y += (1 * speedofEnemy);
		if (vec2NumMicroSteps.y >= cSettings->NUM_STEPS_PER_TILE_YAXIS)
		{
			vec2NumMicroSteps.y = 0;
			vec2Index.y++;
		}
		Direction = UP;

		// Constraint the enemy2D's position within the screen boundary
		Constraint(UP);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(UP) == false)
		{
			vec2NumMicroSteps.y = 0;
		}
	}
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CEnemyBase::PreRender(void)
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
void CEnemyBase::Render(void)
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
void CEnemyBase::PostRender(void)
{
	if (!bIsActive)
		return;

	// Disable blending
	glDisable(GL_BLEND);
}
