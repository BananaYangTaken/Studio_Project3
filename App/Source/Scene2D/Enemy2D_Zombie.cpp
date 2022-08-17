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
	AStarCalculate = true;
	Health = 60;
	Death = 0;
	InvulnerabilityFrame = 0;
	Direction = 2;

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
bool CEnemy2D_Zombie::Init(int x, int y)
{
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

	// Set the start position of the enemy to iRow and iCol
	vec2Index = glm::vec2(x, y);

	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/ZombieSprites.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/ZombieSprites.png" << endl;
		return false;
	}

	//CS:Create Animated Sprites and setup animation


	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(9, 9, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idle", 7, 9);
	animatedSprites->AddAnimation("move", 0, 29);
	animatedSprites->AddAnimation("attack", 30, 69);
	animatedSprites->AddAnimation("death", 72, 77);

	deathTimer = 0;

	runtimeColour = glm::vec4(1, 1, 1, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	UpdateDirection();

	return true;
}

void CEnemy2D_Zombie::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;
	if (InvulnerabilityFrame > 0)
	{
		InvulnerabilityFrame -= dElapsedTime;
	}
	if (deathTimer > 2)
	{
		bIsActive = false;
	}
	if (Health == 0)
	{
		deathTimer = 1;
		sCurrentFSM = static_cast<CEnemyBase::FSM>(DEATH);
	}
	
	switch (sCurrentFSM)
	{
	case IDLE:
		//FSM Transition
		if (iFSMCounter > iMaxFSMCounter)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(PATROL);
			iFSMCounter = 0;
			//cout << "Switching to Patrol State" << endl;
		}
		iFSMCounter++;
		//Animation
		animatedSprites->PlayAnimation("idle", -1, 1.0f);
		break;
	case PATROL:
		//FSM Transition
		if (iFSMCounter > iMaxFSMCounter)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(IDLE);
			iFSMCounter = 0;
		}
		else if (cPhysics2D.CalculateDistance(vec2Index, Player->vec2Index) < 5.0f)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(ATTACK);
			iFSMCounter = 0;
		}
		else
		{
			// Update the Enemy2D's position for patrol
			UpdatePosition();
		}
		iFSMCounter++;
		//Animation
		animatedSprites->PlayAnimation("move", -1, 1.0f);
		break;
	case ATTACK:
		//FSM Transition
		if (cPhysics2D.CalculateDistance(vec2Index, Player->vec2Index) < 7.0f)
		{
			if (AStarCalculate == true)
			{
				auto path = cMap2D->PathFind(vec2Index, Player->vec2Index, heuristic::euclidean, 10);
				//Calculate New Destination
				bool bFirstPosition = true;
				for (const auto& coord : path)
				{
					//std::cout << coord.x << "," << coord.y << "\n";
					if (bFirstPosition == true)
					{
						//Set a destination
						vec2Destination = coord;
						//Calculate the direction between enemy2D and this destination
						vec2Direction = vec2Destination - vec2Index;
						bFirstPosition = false;
					}
					else
					{
						if ((coord - vec2Destination) == vec2Direction)
						{
							//Set a destination
							vec2Destination = coord;
						}
						else
							break;
					}
				}
			}
			else if (cKeyboardController->IsKeyDown(GLFW_KEY_A) ||
				cKeyboardController->IsKeyDown(GLFW_KEY_S) ||
				cKeyboardController->IsKeyDown(GLFW_KEY_D) ||
				cKeyboardController->IsKeyDown(GLFW_KEY_W))
			{
				AStarCalculate = false;
			}
			if (Attack == false)
			{
				UpdatePosition();
			}
			// Attack
			if (cPhysics2D.CalculateDistance(vec2Index, Player->vec2Index, 'x') < 0.5f && Attack == false)
			{
				Attack = true;
				AttackAnim = 1;
				UpdateDirection();

				animatedSprites->PlayAnimation("attack", 1, 1.0f);
				CSoundController::GetInstance()->PlaySoundByID(15);
			}
			if (Attack && AttackAnim <= 0)
			{
				Attack = false;
				sCurrentFSM = static_cast<CEnemyBase::FSM>(RELOAD);
				ReloadDuration = 0.3;
				if (cPhysics2D.CalculateDistance(vec2Index, Player->vec2Index, 'x') < 1.5f)
				{
					if (Player->GetInvulnerabilityFrame() <= 0)
					{
						Player->SetHealth(Player->GetHealth() - 1);
						Player->SetInvulnerabilityFrame(0.5);
						CSoundController::GetInstance()->PlaySoundByID(5);
					}
				}
			}
			else
			{
				AttackAnim -= dElapsedTime;
			}
		}
		else
		{
			if (iFSMCounter > iMaxFSMCounter)
			{
				sCurrentFSM = static_cast<CEnemyBase::FSM>(PATROL);
				iFSMCounter = 0;
				//cout << "ATTACK : Reset counter: " << iFSMCounter << endl;
			}
			iFSMCounter++;
		}
		//Animation
		if (!Attack)
		{
			animatedSprites->PlayAnimation("move", -1, 1.0f);
		}
		break;
	case RELOAD:
		//FSM Transition
		iFSMCounter = 0;
		if (ReloadDuration <= 0)
		{
			ReloadDuration = 0;
			sCurrentFSM = static_cast<CEnemyBase::FSM>(ATTACK);
		}
		else
		{
			ReloadDuration -= dElapsedTime;
		}
		//Animation
		animatedSprites->PlayAnimation("idle", -1, 1.0f);
		break;
	case DEATH:
		animatedSprites->PlayAnimation("death", 1, 1.0f);
		deathTimer += dElapsedTime;
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

			//Check if Map View/Camera at Borders
			if (Player->vec2Index.x < cSettings->VIEW_TILES_XAXIS * 0.5) // Left Side Border
			{
				ScreenPos.x = vec2Index.x + 1;
			}
			else if (Player->vec2Index.x > (cSettings->NUM_TILES_XAXIS - (cSettings->VIEW_TILES_XAXIS * 0.5))) //Right Side Border
			{
				ScreenPos.x = cSettings->VIEW_TILES_XAXIS - (cSettings->NUM_TILES_XAXIS - vec2Index.x) + 1;
			}

			if (Player->vec2Index.y > (cSettings->NUM_TILES_YAXIS - (cSettings->VIEW_TILES_YAXIS * 0.5))) //Top Side Border
			{
				ScreenPos.y = cSettings->VIEW_TILES_YAXIS - (cSettings->NUM_TILES_YAXIS - vec2Index.y) + 1;
			}
			else if (Player->vec2Index.y < cSettings->VIEW_TILES_YAXIS * 0.5) //Bottom Side Border
			{
				ScreenPos.y = vec2Index.y + 1;
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
			vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, ScreenPos.x - 1, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
			vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, ScreenPos.y - 1, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
		}
		else
		{
			vec2UVCoordinate = glm::vec2(2, 2);
		}
	}
}