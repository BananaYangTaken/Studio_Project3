#include "Enemy2D_Creeper.h"

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
CEnemy2D_Creeper::CEnemy2D_Creeper(void)
{
	bIsActive = false;
	cMap2D = NULL;
	cSettings = NULL;
	Player = NULL;
	sCurrentFSM = static_cast<CEnemyBase::FSM>(FSM::IDLE);
	iFSMCounter = 0;
	animatedSprites = NULL;
	AStarCalculate = true;
	Health = 3;
	InvulnerabilityFrame = 0;
	Stunned = false;
	StunDuration = 0;
	Direction = 2;
	Attack = false;
	AttackAnim = 0;

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
CEnemy2D_Creeper::~CEnemy2D_Creeper(void)
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
bool CEnemy2D_Creeper::Init(int x, int y, std::vector<CProjectile2D*>* cProjectileList)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();
	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();


	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(303, uiRow, uiCol) == false)
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


	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Creeper_SpriteSheet.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Creeper_SpriteSheet.png" << endl;
		return false;
	}

	//CS:Create Animated Sprites and setup animation


	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(7, 9, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("up", 19, 26);
	animatedSprites->AddAnimation("left", 28, 35);
	animatedSprites->AddAnimation("down", 37, 44);
	animatedSprites->AddAnimation("right", 46, 53);
	animatedSprites->AddAnimation("idleright", 45, 45);
	animatedSprites->AddAnimation("idleleft", 27, 27);
	animatedSprites->AddAnimation("idledown", 36, 36);
	animatedSprites->AddAnimation("idleup", 18, 18);

	animatedSprites->AddAnimation("AttackLeft", 0, 7);
	animatedSprites->AddAnimation("AttackRight", 10, 17);
	animatedSprites->AddAnimation("Explosion", 54, 62);

	Direction = -1;

	runtimeColour = glm::vec4(0.3, 0.3, 0.3, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	return true;
}


void CEnemy2D_Creeper::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;
	if (InvulnerabilityFrame > 0)
	{
		InvulnerabilityFrame -= dElapsedTime;
	}
	switch (sCurrentFSM)
	{
	case IDLE:
		//FSM Transition
		if (!Stunned)
		{
			if (iFSMCounter > iMaxFSMCounter)
			{
				sCurrentFSM = static_cast<CEnemyBase::FSM>(PATROL);
				iFSMCounter = 0;
				//cout << "Switching to Patrol State" << endl;
			}
			iFSMCounter++;
		}
		else if (Stunned)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(STUNNED);
			StunDuration = 0.5;
		}
		//Animation
		{
			if (Direction == 0)
			{
				animatedSprites->PlayAnimation("idleleft", -1, 1.0f);
			}
			else if (Direction == 1)
			{
				animatedSprites->PlayAnimation("idleright", -1, 1.0f);
			}
			else if (Direction == 2)
			{
				animatedSprites->PlayAnimation("idleup", -1, 1.0f);
			}
			else if (Direction == 3)
			{
				animatedSprites->PlayAnimation("idledown", -1, 1.0f);
			}
		}
		break;
	case PATROL:
		//FSM Transition
		if (!Stunned)
		{
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
		}
		else if (Stunned)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(STUNNED);
			StunDuration = 0.5;
		}
		//Animation
		{
			if (Direction == 0)
			{
				animatedSprites->PlayAnimation("left", -1, 1.0f);
			}
			else if (Direction == 1)
			{
				animatedSprites->PlayAnimation("right", -1, 1.0f);
			}
			else if (Direction == 2)
			{
				animatedSprites->PlayAnimation("up", -1, 1.0f);
			}
			else if (Direction == 3)
			{
				animatedSprites->PlayAnimation("down", -1, 1.0f);
			}
		}
		break;
	case ATTACK:
		//FSM Transition
		if (!Stunned)
		{
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
					cKeyboardController->IsKeyDown(GLFW_KEY_W) ||
					cKeyboardController->IsKeyDown(GLFW_KEY_LEFT) ||
					cKeyboardController->IsKeyDown(GLFW_KEY_RIGHT) ||
					cKeyboardController->IsKeyDown(GLFW_KEY_UP) ||
					cKeyboardController->IsKeyDown(GLFW_KEY_DOWN))
				{
					AStarCalculate = false;
				}
				if (Attack == false)
				{
					UpdatePosition();
				}
				// Attack
				if (cPhysics2D.CalculateDistance(vec2Index, Player->vec2Index, 'x') < 1.5f && Attack == false)
				{
					Attack = true;
					AttackAnim = 2;
					UpdateDirection();
					if (Direction == 0)
					{
						animatedSprites->PlayAnimation("AttackLeft", -1, AttackAnim);
					}
					else if (Direction == 1)
					{
						animatedSprites->PlayAnimation("AttackRight", -1, AttackAnim);
					}
					CSoundController::GetInstance()->PlaySoundByID(15);
				}
				if (Attack && AttackAnim <= 0)
				{
					Attack = false;
					sCurrentFSM = static_cast<CEnemyBase::FSM>(EXPLOSION);
					ExplosionDuration = 2;

					if (vec2Index.y == Player->vec2Index.y
						&& (vec2Index.x + 1 >= Player->vec2Index.x || vec2Index.x - 1 <= Player->vec2Index.x))
					{
						if (Player->GetInvulnerabilityFrame() <= 0)
						{
							Player->SetHealth(Player->GetHealth() - 3);
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
		}
		else if (Stunned)
		{
			sCurrentFSM = static_cast<CEnemyBase::FSM>(STUNNED);
			StunDuration = 0.5;
		}
		//Animation
		if (!Attack)
		{
			if (Direction == 0)
			{
				animatedSprites->PlayAnimation("left", -1, 1.0f);
			}
			else if (Direction == 1)
			{
				animatedSprites->PlayAnimation("right", -1, 1.0f);
			}
			else if (Direction == 2)
			{
				animatedSprites->PlayAnimation("up", -1, 1.0f);
			}
			else if (Direction == 3)
			{
				animatedSprites->PlayAnimation("down", -1, 1.0f);
			}
		}
		break;
	case STUNNED:
		runtimeColour = glm::vec4(1.0, 1.0, 0.0, 1.0);
		//FSM Transition
		iFSMCounter = 0;
		if (StunDuration <= 0)
		{
			Stunned = !Stunned;
			StunDuration = 0;
			runtimeColour = glm::vec4(0.2, 0.2, 0.2, 1.0);
			sCurrentFSM = static_cast<CEnemyBase::FSM>(IDLE);
		}
		else
		{
			StunDuration -= dElapsedTime;
		}
		//Animation
		{
			if (Direction == 0)
			{
				animatedSprites->PlayAnimation("idleleft", -1, 1.0f);
			}
			else if (Direction == 1)
			{
				animatedSprites->PlayAnimation("idleright", -1, 1.0f);
			}
			else if (Direction == 2)
			{
				animatedSprites->PlayAnimation("idleup", -1, 1.0f);
			}
			else if (Direction == 3)
			{
				animatedSprites->PlayAnimation("idledown", -1, 1.0f);
			}
		}
		break;
	case EXPLOSION:
		if (ExplosionDuration == 2)
		{
			CSoundController::GetInstance()->PlaySoundByID(16);
			animatedSprites->PlayAnimation("Explosion", -1, 2.0f);
			ExplosionDuration -= dElapsedTime;
		}
		else if (ExplosionDuration <= 0)
		{
			Health = 0;
		}
		else
		{
			ExplosionDuration -= dElapsedTime;
		}

	default:
		break;
	}

	// Update Jump or Fall
	UpdateJumpFall(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
	animatedSprites->Update(dElapsedTime);
}