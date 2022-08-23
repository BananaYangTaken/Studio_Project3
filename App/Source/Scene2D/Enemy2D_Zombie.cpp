#include "Enemy2D_Zombie.h"

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
#include "../MyMath.h"
#include "Player2D_V2.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CEnemy2D_Zombie::CEnemy2D_Zombie(void)
{
	bIsActive = false;
	cMap2D = NULL;
	cSettings = NULL;
	Player = NULL;
	sCurrentFSM = static_cast<CEnemyBase::FSM>(FSM::IDLE);
	iFSMCounter = 0;
	animatedSprites = NULL;
	AStarCalculate = false;
	Health = prevHP = 60;
	Death = 0;
	InvulnerabilityFrame = 0;
	Direction = 2;
	speedMultiplier = 0.5;
	Attack = false;
	redTimer = 0;

	transform = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::vec2(0);

	// Initialise vecNumMicroSteps
	vec2NumMicroSteps = glm::vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	vec2Destination = glm::vec2(0, 0);	// Initialise the iDestination
	vec2Direction = glm::vec2(0, 0);		// Initialise the iDirection

}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CEnemy2D_Zombie::~CEnemy2D_Zombie(void)
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
bool  CEnemy2D_Zombie::checkforLOS()
{
	// Store the old player position
	Playervec2OldIndex = dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index;
	int distfromplayer = cPhysics2D.CalculateDistance(dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index, vec2Index) + 1;
	int distcorrection = (distfromplayer / 7) - 1;
	distfromplayer = distfromplayer - distcorrection;
	float Xdiff = Playervec2OldIndex.x - vec2Index.x;
	float Ydiff = Playervec2OldIndex.y - vec2Index.y;
	float diffratio = Xdiff / Ydiff;
	int blockscounted = 0, Xcount = 0, Ycount = 0;
	//if the player is to the left or right of the enemy2D
	// check LOS
	if (distfromplayer >= DetectionRadius)
	{
		return false;
	}
	//TOP RIGHT VIEW
	if (vec2Direction.x < 0 && vec2Direction.y < 0)
	{
		for (int i = 0; i < distfromplayer; i++)
		{
			//cMap2D->SetMapInfo(Playervec2OldIndex.y + Ycount, Playervec2OldIndex.x + Xcount, 100);
			blockscounted++;
			Xcount++;
			if (blockscounted >= diffratio)
			{
				Ycount++;
			}
			if (cMap2D->GetMapInfo(Playervec2OldIndex.y + Ycount, Playervec2OldIndex.x + Xcount) >= 100)
			{
				return false;
			}
		}
		return true;
	}


	// TOP LEFT VIEW
	else if (vec2Direction.x > 0 && vec2Direction.y < 0)
	{
		for (int i = 0; i < distfromplayer; i++)
		{
			blockscounted++;
			Xcount--;
			
			if (blockscounted >= diffratio)
			{
				Ycount++;
			}
			if (cMap2D->GetMapInfo(Playervec2OldIndex.y + Ycount, Playervec2OldIndex.x + Xcount) >= 100)
			{
				return false;
			}
		}
		return true;
	}


	// BOTTOM LEFT VIEW
	else if (vec2Direction.x > 0 && vec2Direction.y > 0)
	{
		for (int i = 0; i < distfromplayer; i++)
		{
			blockscounted++;
			Xcount--;
			if (blockscounted >= diffratio)
			{
				Ycount--;
			}
			if (cMap2D->GetMapInfo(Playervec2OldIndex.y + Ycount, Playervec2OldIndex.x + Xcount) >= 100)
			{
				return false;
			}
		}
		return true;
	}

	// BOTTOM RIGHT VIEW
	else if (vec2Direction.x < 0 && vec2Direction.y > 0)
	{
		for (int i = 0; i < distfromplayer; i++)
		{
			blockscounted++;
			Xcount++;
			if (blockscounted >= diffratio)
			{
				Ycount--;
			}
			if (cMap2D->GetMapInfo(Playervec2OldIndex.y + Ycount, Playervec2OldIndex.x + Xcount) >= 100)
			{
				return false;
			}
		}
		return true;
	}

	else if (vec2Direction.x == 0 && vec2Direction.y >= 0)
	{
		for (int i = 0; i < distfromplayer; i++)
		{
			blockscounted++;
			Ycount--;
			if (cMap2D->GetMapInfo(Playervec2OldIndex.y + Ycount, Playervec2OldIndex.x) >= 100)
			{
				return false;
			}
		}
		return true;
	}

	else if ( vec2Direction.x == 0 && vec2Direction.y <= 0)
	{
	for (int i = 0; i < distfromplayer; i++)
	{
		blockscounted++;
		Ycount++;
		if (cMap2D->GetMapInfo(Playervec2OldIndex.y + Ycount, Playervec2OldIndex.x) >= 100)
		{
			return false;
		}
	}
	return true;
	}

	else if (vec2Direction.x <= 0 && vec2Direction.y == 0)
	{
		for (int i = 0; i < distfromplayer; i++)
		{
			blockscounted++;
			Xcount++;
			if (cMap2D->GetMapInfo(Playervec2OldIndex.y, Playervec2OldIndex.x + Xcount) >= 100)
			{
				return false;
			}
		}
		return true;
	}
	else if (vec2Direction.x >= 0 && vec2Direction.y == 0)
	{
	for (int i = 0; i < distfromplayer; i++)
	{
		blockscounted++;
		Xcount--;
		if (cMap2D->GetMapInfo(Playervec2OldIndex.y, Playervec2OldIndex.x + Xcount) >= 100)
		{
			return false;
		}
	}
	return true;
	}
}

bool CEnemy2D_Zombie::Init(int x, int y)
{
	cout << "INIT INSTANCE ZOMBIE NORMAL " << endl;
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();
	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();

	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(302, uiRow, uiCol) == false)
	{
		return false;	// Unable to find the start position of the enemy, so quit this game
	}
	// Erase the value of the enemy in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);
	cout << "ZOMBIE SET ON MAP" << endl;
	// Set the start position of the enemy to iRow and iCol
	vec2Index = glm::vec2(x, y);

	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	cout << "Loading textureID" << endl;
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/regularZombie.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/regularZombie.png" << endl;
		return false;
	}
	Rotation = 0;
	//CS:Create Animated Sprites and setup animation

	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(4, 17, (cSettings->TILE_WIDTH + cSettings->TILE_HEIGHT) * 0.5, (cSettings->TILE_WIDTH + cSettings->TILE_HEIGHT) * 0.5);
	animatedSprites->AddAnimation("idle", 0, 16);
	animatedSprites->AddAnimation("move", 17, 33);
	animatedSprites->AddAnimation("attack", 34, 42);
	animatedSprites->AddAnimation("death", 51, 56);
	deathTimer = 0;

	runtimeColour = glm::vec4(1, 1, 1, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	UpdateDirection();
	return true;
}
void CEnemy2D_Zombie::UpdateToLastLOS()
{
	// Set the destination to the player
	vec2Destination = OldPositu;

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

void CEnemy2D_Zombie::droploot()
{
	int lootrand = rand() % 6 + 1;
	if (lootrand == 3)
	{
		int lootcount = rand() % 2 + 1;

		for (int i = 0; i < lootcount; i++)
		{
			float recvex = vec2Index.y, recvey = vec2Index.x;
			if (cMap2D->GetMapInfo(recvey, recvex) == 0 || cMap2D->GetMapInfo(recvey, recvey) == 22)
			{
				spawnloot(recvey, recvex);
			}
			else if (cMap2D->GetMapInfo(recvey + 1, recvex) == 0 || cMap2D->GetMapInfo(recvey + 1, recvey) == 22)
			{
				spawnloot(recvey + 1, recvex);
			}
			else if (cMap2D->GetMapInfo(recvey - 1, recvex) == 0 || cMap2D->GetMapInfo(recvey - 1, recvey) == 22)
			{
				spawnloot(recvey - 1, recvex);
			}
			else if (cMap2D->GetMapInfo(recvey, recvex + 1) == 0 || cMap2D->GetMapInfo(recvey, recvey + 1) == 22)
			{
				spawnloot(recvey, recvex + 1);
			}
			else if (cMap2D->GetMapInfo(recvey, recvex - 1) == 0 || cMap2D->GetMapInfo(recvey, recvey - 1) == 22)
			{
				spawnloot(recvey, recvex - 1);
			}
			else if (cMap2D->GetMapInfo(recvey + 1, recvex + 1) == 0 || cMap2D->GetMapInfo(recvey + 1, recvey + 1) == 22)
			{
				spawnloot(recvey + 1, recvex + 1);
			}
			else if (cMap2D->GetMapInfo(recvey - 1, recvex - 1) == 0 || cMap2D->GetMapInfo(recvey - 1, recvey - 1) == 22)
			{
				spawnloot(recvey - 1, recvex - 1);
			}
			else if (cMap2D->GetMapInfo(recvey + 1, recvex - 1) == 0 || cMap2D->GetMapInfo(recvey + 1, recvey - 1) == 22)
			{
				spawnloot(recvey + 1, recvex - 1);
			}
			else if (cMap2D->GetMapInfo(recvey - 1, recvex + 1) == 0 || cMap2D->GetMapInfo(recvey - 1, recvey + 1) == 22)
			{
				spawnloot(recvey - 1, recvex + 1);
			}
		}
	}
}


void CEnemy2D_Zombie::spawnloot(float vecX, float vecY)
{
	int loottype = rand() % 5 + 1;
	if(loottype == 1) cMap2D->SetMapInfo(vecX, vecY, 30); // Bandagge
	else if (loottype == 2) cMap2D->SetMapInfo(vecX, vecY, 40); // Scrap Metal
	else if (loottype == 3) cMap2D->SetMapInfo(vecX, vecY, 41); // Stone Ore
	else if (loottype == 4) cMap2D->SetMapInfo(vecX, vecY, 33); // Fabric
	else if (loottype == 5) cMap2D->SetMapInfo(vecX, vecY, 34); // Wood
}
void CEnemy2D_Zombie::Update(const double dElapsedTime)
{
	vec2OldIndex = vec2Index;
	UpdateDirection();
	checkforLOS();
	if (!bIsActive)
		return;
	if (Health <= 0)
	{
		if (deathTimer > 2)
		{
			bIsActive = false;
			droploot();
		}
		if (deathTimer == 0)
		{
			deathTimer = 1;
			sCurrentFSM = static_cast<CEnemyBase::FSM>(DEATH);
		}
	}

	//Update Invulnerable frames
	if (InvulnerabilityFrame < 0)
	{
		InvulnerabilityFrame = 0;
	}
	else
	{
		InvulnerabilityFrame -= dElapsedTime;
	}
	

	unsigned int IRow;
	unsigned int ICol;

	if (cMap2D->CheckValue(cSettings->NUM_TILES_YAXIS - vec2Index.y, vec2Index.x, 24))
	{
		sCurrentFSM = static_cast<CEnemyBase::FSM>(SLOWED);
	}
	else if(sCurrentFSM == static_cast<CEnemyBase::FSM>(SLOWED))
	{
		sCurrentFSM= static_cast<CEnemyBase::FSM>(CHASE);
	}
	
	
	switch (sCurrentFSM)
	{
	case IDLE:
		//FSM Transition
		if (iFSMCounter > iMaxFSMCounter)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(PATROL);
			iFSMCounter = 0;
		}
		iFSMCounter++;
		//Animation
		animatedSprites->PlayAnimation("idle", -1, 1.0f);
		runtimeColour = glm::vec4(1, 1, 1, 1.0);
		break;
	case PATROL:
		if (iFSMCounter / 10 > iMaxFSMCounter)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(IDLE);
			iFSMCounter = 0;
			randomDir();
		}
		else if (checkforLOS() == true && cPhysics2D.CalculateDistance(vec2Index, dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index) <= DetectionRadius)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(CHASE);
			iFSMCounter = 0;
		}
		else if (Health <= 30)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(RETREAT);
			CSoundController::GetInstance()->PlaySoundByID(35);
			iFSMCounter = 0;
		}
		else
		{
			// Patrol around
			// Update the Enemy2D's position for patrol
			randomiseDirectionMove(speedMultiplier);
		}
		iFSMCounter++;
		//Animation
		animatedSprites->PlayAnimation("move", -1, 1.0f);
		runtimeColour = glm::vec4(1, 1, 1, 1.0);
		break;
	case CHASE:
		//FSM Transition
		if (cPhysics2D.CalculateDistance(vec2Index, dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index) < 1.5f)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(ATTACK);
			iFSMCounter = 0;
			bool bFirstPosition = true;
		}
		else if(checkforLOS() == true && cPhysics2D.CalculateDistance(vec2Index, dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index) <= DetectionRadius)
		{
			idlecount++;
			hasseenplayeronce = true;
			if (idlecount >= 15)
			{				
				OldPositu = dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index;
				idlecount = 0;
			}
			UpdateDirection();
			UpdatePosition(speedMultiplier);
		}
		else if(checkforLOS() == false && hasseenplayeronce == true 
			|| checkforLOS() == true && cPhysics2D.CalculateDistance(vec2Index, dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index) > DetectionRadius)
		{
			UpdateToLastLOS();
			UpdatePosition(speedMultiplier);
		}
		else if (hasseenplayeronce == false)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(PATROL);
		}
		if (vec2Index == OldPositu)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(PATROL);
		}
		iFSMCounter++;
		//Animation
		animatedSprites->PlayAnimation("move", -1, 1.0f);
		runtimeColour = glm::vec4(1, 1, 1, 1.0);
		break;
	case ATTACK:
		//FSM Transition
		if (cPhysics2D.CalculateDistance(vec2Index, dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index) < 1.5f)
		{
			if (Attack == false)
			{
				// Attack
				Attack = true;
				animatedSprites->PlayAnimation("attack", 1, 1.0f);
			}
			else if (Attack && AttackAnim <= 0)
			{
				Attack = false;
				sCurrentFSM = static_cast<CEnemyBase::FSM>(RELOAD);
				ReloadDuration = 1;

				if (dynamic_cast<CPlayer2D_V2*>(Player)->GetInvulnerabilityFrame() <= 0)
				{
					dynamic_cast<CPlayer2D_V2*>(Player)->SetHealth(dynamic_cast<CPlayer2D_V2*>(Player)->GetHealth() - 10);
					dynamic_cast<CPlayer2D_V2*>(Player)->SetInvulnerabilityFrame(0.5);
				}
				
			}
		}
		else
		{
			if (iFSMCounter > iMaxFSMCounter)
			{
				sCurrentFSM = static_cast<CEnemyBase::FSM>(PATROL);
				iFSMCounter = 0;
			}
			iFSMCounter++;
		}
		runtimeColour = glm::vec4(1, 1, 1, 1.0);
		break;
	case RELOAD:
		//FSM Transition
		if (ReloadDuration <= 0)
		{
			ReloadDuration = 0;
			Attack = false;
			sCurrentFSM = static_cast<CEnemyBase::FSM>(CHASE);
		}
		else
		{
			ReloadDuration -= dElapsedTime;
		}
		runtimeColour = glm::vec4(1, 1, 1, 1.0);
		break;
	case SLOWED:
		//FSM Transition
		if (cPhysics2D.CalculateDistance(vec2Index, dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index) < 1.5f)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(ATTACK);
			iFSMCounter = 0;
			bool bFirstPosition = true;
		}
		else if (checkforLOS() == true && cPhysics2D.CalculateDistance(vec2Index, dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index) <= DetectionRadius)
		{
			idlecount++;
			hasseenplayeronce = true;
			if (idlecount >= 15)
			{
				OldPositu = dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index;
				idlecount = 0;
			}
			UpdateDirection();
			UpdatePositionSlowed(speedMultiplier);
		}
		else if (checkforLOS() == false && hasseenplayeronce == true
			|| checkforLOS() == true && cPhysics2D.CalculateDistance(vec2Index, dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index) > DetectionRadius)
		{
			UpdateToLastLOS();
			UpdatePositionSlowed(speedMultiplier);
		}
		else if (hasseenplayeronce == false)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(IDLE);
		}
		iFSMCounter++;
		//Animation
		animatedSprites->PlayAnimation("move", -1, 1.0f);
		runtimeColour = glm::vec4(1, 1, 1, 1.0);
		break;
	case DEATH:
		animatedSprites->PlayAnimation("death", 1, 1.0f);
		deathTimer += dElapsedTime;
		break;
	default:
		break;
	}
	if (prevHP > Health)
	{
		redTimer = 0.3;
	}
	if (redTimer > 0)
	{
		runtimeColour = glm::vec4(1, 0, 0, 1.0);
		redTimer -= dElapsedTime;
	}
	prevHP = Health;
	++iFSMCounter;
	animatedSprites->Update(dElapsedTime);
	//wood baricase checker
	int chance = Math::RandIntMinMax(1, 600);
	if (Level == 1)
	{
		//Top window if outside
		if (vec2Index.y >= 50 && vec2Index.y <= 51)
		{
			if (vec2Index.x == 9 || vec2Index.x == 17)
			{
				if (chance == 1)
				{
					vec2Index.y = 48;
				}
			}
		}
		//Top window if inside
		if (vec2Index.y <= 50 && vec2Index.y >= 49)
		{
			if (vec2Index.x == 9 || vec2Index.x == 17)
			{
				if (chance == 1)
				{
					vec2Index.y = 52;
				}
			}
		}

		//Bot window if inside
		if (vec2Index.y >= 36 && vec2Index.y <= 37)
		{
			if (vec2Index.x == 9 || vec2Index.x == 17)
			{
				if (chance == 1)
				{
					vec2Index.y = 34;
				}
			}
		}
		////Bot window if outside
		if (vec2Index.y <= 36 && vec2Index.y >= 35)
		{
			if (vec2Index.x == 9 || vec2Index.x == 17)
			{
				if (chance == 1)
				{
					vec2Index.y = 38;
				}
			}
		}
	}
	// Update the UV Coordinates
	{
		//if within viewing distance
		if ((abs(dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index.x - vec2Index.x) <= cSettings->VIEW_TILES_XAXIS * 0.5 + 1) && (abs(dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index.y - vec2Index.y) <= cSettings->VIEW_TILES_YAXIS * 0.5 + 1))
		{
			//Calculate Position of Entity on Screen
			glm::vec2 ScreenPos = glm::vec2(0, 0);
			glm::vec2 TotalMicroStep = vec2NumMicroSteps;
			//Check if Map View/Camera at Borders
			if (dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index.x < cSettings->VIEW_TILES_XAXIS * 0.5) // Left Side Border
			{
				ScreenPos.x = vec2Index.x + 1;
			}
			else if (dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index.x > (cSettings->NUM_TILES_XAXIS - (cSettings->VIEW_TILES_XAXIS * 0.5))) //Right Side Border
			{
				ScreenPos.x = cSettings->VIEW_TILES_XAXIS - (cSettings->NUM_TILES_XAXIS - vec2Index.x) + 1;
			}
			else
			{
				TotalMicroStep.x -= dynamic_cast<CPlayer2D_V2*>(Player)->vec2NumMicroSteps.x;
			}

			if (dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index.y > (cSettings->NUM_TILES_YAXIS - (cSettings->VIEW_TILES_YAXIS * 0.5))) //Top Side Border
			{
				ScreenPos.y = cSettings->VIEW_TILES_YAXIS - (cSettings->NUM_TILES_YAXIS - vec2Index.y) + 1;
			}
			else if (dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index.y < cSettings->VIEW_TILES_YAXIS * 0.5) //Bottom Side Border
			{
				ScreenPos.y = vec2Index.y + 1;
			}
			else
			{
				TotalMicroStep.y -= dynamic_cast<CPlayer2D_V2*>(Player)->vec2NumMicroSteps.y;
			}


			//If not at Border, Entity at Center of Screen displaced by x:1 y:1
			if (ScreenPos.x == 0)
			{
				ScreenPos.x = vec2Index.x + 1 - dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index.x + cSettings->VIEW_TILES_XAXIS * 0.5;
			}
			if (ScreenPos.y == 0)
			{
				ScreenPos.y = vec2Index.y + 1 - dynamic_cast<CPlayer2D_V2*>(Player)->vec2Index.y + cSettings->VIEW_TILES_YAXIS * 0.5;
			}

			//Convert position to UV Coords
			vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, ScreenPos.x - 1, false, TotalMicroStep.x * cSettings->MICRO_STEP_XAXIS);
			vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, ScreenPos.y - 1, false, TotalMicroStep.y * cSettings->MICRO_STEP_YAXIS);
		}
		else
		{
			vec2UVCoordinate = glm::vec2(2, 2);
		}
	}
	// Update the Rotation
	{
		if (vec2OldIndex != vec2Index)
			Rotation = cPhysics2D.CalculateRotation(vec2OldIndex, vec2OldIndex + glm::vec2(1, 0), vec2Index);
	}
}

