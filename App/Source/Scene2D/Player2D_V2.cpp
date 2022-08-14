﻿/**
 Player2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Player2D_V2.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
#include "Primitives/MeshBuilder.h"
#include "Projectile2DManager.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D_V2::CPlayer2D_V2(void)
	: cMap2D(NULL)
	, Player(NULL)
	, OnLadder(NULL)
	, cKeyboardController(NULL)
	, animatedSprites(NULL)
	, runtimeColour(glm::vec4(1.0f))
	, Direction(DOWN)
	, jumpcount(NULL)
	, Prejumpcount(NULL)
	, teleportpower(NULL)
	, jumpheightmodifier(NULL)
	, speedmodifier(NULL)
	, Timer(NULL)
	, PhaseThroughWall(NULL)
	, PhaseTimer(NULL)
	, TapCount(NULL)
	, DoubleTapTimer(NULL)
	, PreviousTapDirection(NULL)
	, CurrentTapDirection(NULL)
	, AtExit(NULL)
	, cObjectList(NULL)
	, cProjectileList(NULL)
	, Health(NULL)
	, InvulnerabilityFrame(NULL)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::vec2(0);

	// Initialise vecNumMicroSteps
	vec2NumMicroSteps = glm::vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D_V2::~CPlayer2D_V2(void)
{
	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	if (animatedSprites)
	{
		delete animatedSprites;
		animatedSprites = NULL;
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

void CPlayer2D_V2::SetPlayer(unsigned int newPlayer)
{
	Player = newPlayer;
}

unsigned int CPlayer2D_V2::GetPlayer(void)
{
	return Player;
}

/**
  @brief Initialise this instance
  */
void CPlayer2D_V2::SetObjectList(std::vector<CObject2D*>* newObjectList)
{
	cObjectList = newObjectList;
}
void CPlayer2D_V2::SetProjectileList(std::vector<CProjectile2D*>* newProjectileList)
{
	cProjectileList = newProjectileList;
}
void CPlayer2D_V2::SetLoadObject(bool newState)
{
	LoadObject = newState;
}
bool CPlayer2D_V2::GetLoadObject(void)
{
	return LoadObject;
}
bool CPlayer2D_V2::Init(void)
{
	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();

	// Load the sounds into CSoundController
	cSoundController = CSoundController::GetInstance();

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();

	// Get the handler to the CGUI_Scene2D instance
	cGUI_Scene2D = CGUI_Scene2D::GetInstance();

	// Get the handler to the CInventoryManager instance
	cInventoryManager = CInventoryManager::GetInstance();

	//Get the handler to the Game Manager Instance
	cGameManager = CGameManager::GetInstance();

	cProjectileList = CProjectileManager::GetInstance()->GetProjectileList();

	iJumpCount = 0;

	PhaseTimer = 0;

	PhaseThroughWall = false;

	jumpcount = 1;

	Prejumpcount = jumpcount;

	teleportpower = false;

	jumpheightmodifier = 1;

	speedmodifier = 1;

	Timer = 0;

	AtExit = false;

	TapCount = 0;

	DoubleTapTimer = 0;

	OnLadder = false; 
	
	LoadObject = false;

	Health = 5;

	InvulnerabilityFrame = 0;

	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow;
	unsigned int uiCol;
	if (Player == 1)
	{
		uiRow = -1;
		uiCol = -1;
		if (cMap2D->FindValue(200, uiRow, uiCol) == false)
		{
			return false;	// Unable to find the start position of the player, so quit this game
		}
		// Erase the value of the player in the arrMapInfo
		cMap2D->SetMapInfo(uiRow, uiCol, 0);
		// Set the start position of the Player to iRow and iCol
		vec2Index = glm::vec2(uiCol, uiRow);
		// By default, microsteps should be zero
		vec2NumMicroSteps = glm::vec2(0, 0);
	}
	else if (Player == 2)
	{
		uiRow = -1;
		uiCol = -1;
		if (cMap2D->FindValue(201, uiRow, uiCol) == false)
		{
			return false;	// Unable to find the start position of the player, so quit this game
		}
		// Erase the value of the player in the arrMapInfo
		cMap2D->SetMapInfo(uiRow, uiCol, 0);
		// Set the start position of the Player to iRow and iCol
		vec2Index = glm::vec2(uiCol, uiRow);
		// By default, microsteps should be zero
		vec2NumMicroSteps = glm::vec2(0, 0);
	}


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	if (Player == 1)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/SkeletonArcher_SpriteSheet.png", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/SkeletonArcher_SpriteSheet.png" << endl;
			return false;
		}
	}
	else if (Player == 2)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/HumanKnight_SpriteSheet.png", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/HumanKnight_SpriteSheet.png" << endl;
			return false;
		}
	}

	// Create the quad mesh for the player
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS:Create Animated Sprites and setup animation
	if (Player == 1)
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(6, 13, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
		animatedSprites->AddAnimation("up", 1, 8);
		animatedSprites->AddAnimation("left", 14, 21);
		animatedSprites->AddAnimation("down", 27, 34);
		animatedSprites->AddAnimation("right", 40, 47);
		animatedSprites->AddAnimation("idleup", 0, 0);
		animatedSprites->AddAnimation("idleleft", 52, 52);
		animatedSprites->AddAnimation("idledown", 26, 26);
		animatedSprites->AddAnimation("idleright", 65, 65);

		animatedSprites->AddAnimation("AttackLeft", 52, 61);
		animatedSprites->AddAnimation("AttackRight", 65, 74 );
	}
	else if (Player == 2)
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(6, 9, cSettings->TILE_WIDTH * 3, cSettings->TILE_HEIGHT);
		animatedSprites->AddAnimation("up", 1, 8);
		animatedSprites->AddAnimation("left", 10, 17);
		animatedSprites->AddAnimation("down", 19, 26);
		animatedSprites->AddAnimation("right", 28, 35);
		animatedSprites->AddAnimation("idleright", 27, 27);
		animatedSprites->AddAnimation("idleleft", 9, 9);
		animatedSprites->AddAnimation("idledown", 18, 18);
		animatedSprites->AddAnimation("idleup", 0, 0);

		animatedSprites->AddAnimation("AttackLeft", 38, 41);
		animatedSprites->AddAnimation("AttackRight", 47, 50);
	}
	attack = false;
	attackAnim = 0;


	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	//Item intialise once
	if(Player == 1)
	{
		// Add a Lives as one of the inventory items
		cInventoryItem = cInventoryManager->Add("Lives", "Image/Scene2D_Lives.tga", 3, 3);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
		//// Add a Coins as one of the inventory items
		//cInventoryItem = cInventoryManager->Add("Coins", "Image/Coin.tga", 15, 0);
		//cInventoryItem->vec2Size = glm::vec2(25, 25);
		// Add a Rune as one of the inventory items
		cInventoryItem = cInventoryManager->Add("Rune", "Image/Rune.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
		// Add a Key as one of the inventory items
		cInventoryItem = cInventoryManager->Add("Key", "Image/Key.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
	}
	return true;
}

/**
 @brief Reset this instance
 */
bool CPlayer2D_V2::Reset()
{
	unsigned int uiRow, uiCol;
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
	jumpcount = Prejumpcount;
	teleportpower = false;
	PhaseThroughWall = false;
	PhaseTimer = 0;
	TapCount = 0;
	CurrentTapDirection = PreviousTapDirection = NULL;
	DoubleTapTimer = 0;
	speedmodifier = 1;
	Timer = 0;
	InvulnerabilityFrame = 0;
	Health = 5;
	if (Player == 1)
	{
		uiRow = -1;
		uiCol = -1;
		if (cMap2D->FindValue(200, uiRow, uiCol) == false)
		{
			return false;	// Unable to find the start position of the player 1, so quit this game
		}
		// Set the start position of the Player to iRow and iCol
		vec2Index = glm::vec2(uiCol, uiRow);
		// Erase the value of the player in the arrMapInfo
		cMap2D->SetMapInfo(uiRow, uiCol, 0);
		// By default, microsteps should be zero
		vec2NumMicroSteps = glm::vec2(0, 0);
	}
	if (Player == 2)
	{
		uiRow = -1;
		uiCol = -1;
		if (cMap2D->FindValue(201, uiRow, uiCol) == false && Player == 2)
		{
			return false;	// Unable to find the start position of the player 2, so quit this game
		}
		// Set the start position of the Player to iRow and iCol
		vec2Index = glm::vec2(uiCol, uiRow);
		// Erase the value of the player in the arrMapInfo
		cMap2D->SetMapInfo(uiRow, uiCol, 0);
		// By default, microsteps should be zero
		vec2NumMicroSteps = glm::vec2(0, 0);
	}

	// Reset Rune
	cInventoryItem = cInventoryManager->GetItem("Rune");
	cInventoryItem->iItemCount = 0;
	// Reset Key
	cInventoryItem = cInventoryManager->GetItem("Key");
	cInventoryItem->iItemCount = 0;

	//CS: Init the color to white
	//runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	iJumpCount = 0;

	return true;
}

/**
 @brief Update this instance
 */
void CPlayer2D_V2::Update(const double dElapsedTime)
{
	// Store the old position
	vec2OldIndex = vec2Index;

	if(!attack)
		idle = true;
	if (InvulnerabilityFrame > 0)
	{
		InvulnerabilityFrame -= dElapsedTime;
	}
	else
	{
		InvulnerabilityFrame = 0;
	}
	if (Health <= 0 && InvulnerabilityFrame <= 0)
	{
		if (CInventoryManager::GetInstance()->GetItem("Lives")->GetCount() > 1)
		{
			CInventoryManager::GetInstance()->GetItem("Lives")->Remove(1);
			CGameManager::GetInstance()->bLevelToReplay = true;
		}
		else
		{
			CGameManager::GetInstance()->bPlayerLost = true;
			}
	}
	// Get keyboard updates
	if (Player == 1)
	{
		if (!attack)
		{
			if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
			{
				// Calculate the new position to the left
				if (vec2Index.x >= 0)
				{
					vec2NumMicroSteps.x -= 1 * speedmodifier;
					if (vec2NumMicroSteps.x < 0)
					{
						vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
						vec2Index.x--;
					}
				}
				//If the new position is not feasible, then revert to old position
				if (CheckPosition(LEFT) == false)
				{
					if (PhaseThroughWall == false)
					{
						vec2Index = vec2OldIndex;
						vec2NumMicroSteps.x = 0;
					}
				}
				//Check if the player is in mid-air, suck as walking off a platform
				if (IsMidAir() == true)
				{
					if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
						cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				}

				//Player is not idle
				idle = false;

				//Set Direction
				Direction = 0;


				// Constraint the player's position within the screen boundary
				Constraint(LEFT);

				//CS: Play the "left" animation
				animatedSprites->PlayAnimation("left", -1, 1.0f);

				//CS: Change Color
				runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
			}
			else if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
			{
				// Calculate the new position to the right
				if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
				{
					vec2NumMicroSteps.x += 1 * speedmodifier;

					if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
					{
						vec2NumMicroSteps.x = 0;
						vec2Index.x++;
					}
				}
				//If the new position is not feasible, then revert to old position
				if (CheckPosition(RIGHT) == false)
				{
					if (PhaseThroughWall == false)
					{
						vec2Index = vec2OldIndex;
						vec2NumMicroSteps.x = 0;
					}
				}
				//Check if the player is in mid-air, suck as walking off a platform
				if (IsMidAir() == true)
				{
					if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
						cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				}
				//Player is not idle
				idle = false;

				//Set Direction
				Direction = 1;

				// Constraint the player's position within the screen boundary
				Constraint(RIGHT);

				//CS: Play the "right" animation
				animatedSprites->PlayAnimation("right", -1, 1.0f);

				//CS: Init the color to white
				runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
			}
			if (OnLadder == true)
			{
				if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
				{
					// Calculate the new position up
					if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
					{
						vec2NumMicroSteps.y += 1 * speedmodifier;
						if (vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
						{
							vec2NumMicroSteps.y = 0;
							vec2Index.y++;
						}
					}

					// If the new position is not feasible, then revert to old position
					if (CheckPosition(UP) == false)
					{
						if (PhaseThroughWall == false)
						{
							vec2NumMicroSteps.y = 0;
						}
					}
					//Player is not idle
					idle = false;

					//Set Direction
					Direction = 2;

					// Constraint the player's position within the screen boundary
					Constraint(UP);

					//CS: Play the "up" animation
					animatedSprites->PlayAnimation("up", -1, 1.0f);

				}
				else if (cKeyboardController->IsKeyDown(GLFW_KEY_S))
				{
					// Calculate the new position down
					if (vec2Index.y >= 0)
					{
						vec2NumMicroSteps.y -= 1 * speedmodifier;
						if (vec2NumMicroSteps.y < 0)
						{
							vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
							vec2Index.y--;
						}
					}
					// If the new position is not feasible, then revert to old position
					if (CheckPosition(DOWN) == false)
					{
						vec2Index = vec2OldIndex;
						vec2NumMicroSteps.y = 0;
					}

					//Player is not idle
					idle = false;

					//Set Direction
					Direction = 3;

					// Constraint the player's position within the screen boundary
					Constraint(DOWN);

					//CS: Play the "down" animation
					animatedSprites->PlayAnimation("down", -1, 1.0f);
				}
			}
			else
			{
				if (cKeyboardController->IsKeyPressed(GLFW_KEY_W))
				{
					if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE)
					{
						cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
						cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 2.5f * jumpheightmodifier));
						iJumpCount += 1;
						cSoundController->PlaySoundByID(12);
					}
					else
					{
						if (iJumpCount < jumpcount)
						{
							cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
							cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 2.0f));
							iJumpCount += 1;
							cSoundController->PlaySoundByID(12);
						}
					}
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_SPACE) )
				{
					//Player is not idle
					idle = false;
					if (Direction == 1)
					{
						//CS: Play the "right" animation
						attackAnim = 1;
						animatedSprites->PlayAnimation("AttackRight", -1, attackAnim);
						attack = !attack;
						CSoundController::GetInstance()->PlaySoundByID(14);
					}
					else if (Direction == 0)
					{
						//CS: Play the "left" animation
						attackAnim = 1;
						animatedSprites->PlayAnimation("AttackLeft", -1, attackAnim);
						attack = !attack;
						CSoundController::GetInstance()->PlaySoundByID(14);
					}

					//CS: Init the color to white
					runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
				}
			}
		}
		else if (attack)
		{
			if (attackAnim <= 0)
			{
				attack = !attack;
				CProjectile2D* cProjectile;
				cProjectile = new CProjectile2D;
				cProjectileList->push_back(cProjectile);
				cProjectile->SetShader("Shader2D_Colour");
				cProjectile->Init(vec2Index, vec2NumMicroSteps, Direction, 9, CProjectile2D::FRIENDLY, CProjectile2D::ARROW);
				attackAnim = 0;
			}
			else
			{
				attackAnim -= dElapsedTime;
			}
		}
			
		static bool Button_Pressed1 = false;
		if (!Button_Pressed1 && (cKeyboardController->IsKeyPressed(GLFW_KEY_A) || cKeyboardController->IsKeyPressed(GLFW_KEY_D)))
		{
			if (TapCount == 0)
			{
				if (cKeyboardController->IsKeyPressed(GLFW_KEY_A))
				{
					PreviousTapDirection = 65;
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_D))
				{
					PreviousTapDirection = 68;
				}

			}
			else if (TapCount == 1)
			{
				if (cKeyboardController->IsKeyPressed(GLFW_KEY_A))
				{
					CurrentTapDirection = 65;
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_D))
				{
					CurrentTapDirection = 68;
				}
			}
			Button_Pressed1 = true;
			TapCount++;
		}
		else if (Button_Pressed1 && !(cKeyboardController->IsKeyPressed(GLFW_KEY_A) || cKeyboardController->IsKeyPressed(GLFW_KEY_D)))
		{
			Button_Pressed1 = false;
		}

	}
	else if (Player == 2)
	{
		if (!attack)
		{
			if (cKeyboardController->IsKeyDown(GLFW_KEY_LEFT))
			{
				// Calculate the new position to the left
				if (vec2Index.x >= 0)
				{
					vec2NumMicroSteps.x -= 1 * speedmodifier;
					if (vec2NumMicroSteps.x < 0)
					{
						vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
						vec2Index.x--;
					}
				}
				//If the new position is not feasible, then revert to old position
				if (CheckPosition(LEFT) == false)
				{
					if (PhaseThroughWall == false)
					{
						vec2Index = vec2OldIndex;
						vec2NumMicroSteps.x = 0;
					}
				}
				//Check if the player is in mid-air, suck as walking off a platform
				if (IsMidAir() == true)
				{
					if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
						cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				}

				//Player is not idle
				idle = false;

				//Set Direction
				Direction = 0;


				// Constraint the player's position within the screen boundary
				Constraint(LEFT);

				//CS: Play the "left" animation
				animatedSprites->PlayAnimation("left", -1, 1.0f);

				//CS: Change Color
				runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
			}
			else if (cKeyboardController->IsKeyDown(GLFW_KEY_RIGHT))
			{
				// Calculate the new position to the right
				if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
				{
					vec2NumMicroSteps.x += 1 * speedmodifier;

					if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
					{
						vec2NumMicroSteps.x = 0;
						vec2Index.x++;
					}
				}
				//If the new position is not feasible, then revert to old position
				if (CheckPosition(RIGHT) == false)
				{
					if (PhaseThroughWall == false)
					{
						vec2Index = vec2OldIndex;
						vec2NumMicroSteps.x = 0;
					}
				}
				//Check if the player is in mid-air, suck as walking off a platform
				if (IsMidAir() == true)
				{
					if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
						cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				}
				//Player is not idle
				idle = false;

				//Set Direction
				Direction = 1;

				// Constraint the player's position within the screen boundary
				Constraint(RIGHT);

				//CS: Play the "right" animation
				animatedSprites->PlayAnimation("right", -1, 1.0f);

				//CS: Init the color to white
				runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
			}
			if (OnLadder == true)
			{
				if (cKeyboardController->IsKeyDown(GLFW_KEY_UP))
				{
					// Calculate the new position up
					if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
					{
						vec2NumMicroSteps.y += 1 * speedmodifier;
						if (vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
						{
							vec2NumMicroSteps.y = 0;
							vec2Index.y++;
						}
					}

					// If the new position is not feasible, then revert to old position
					if (CheckPosition(UP) == false)
					{
						vec2NumMicroSteps.y = 0;
					}
					//Player is not idle
					idle = false;

					//Set Direction
					Direction = 2;

					// Constraint the player's position within the screen boundary
					Constraint(UP);

					//CS: Play the "up" animation
					animatedSprites->PlayAnimation("up", -1, 1.0f);

				}
				else if (cKeyboardController->IsKeyDown(GLFW_KEY_DOWN))
				{
					// Calculate the new position down
					if (vec2Index.y >= 0)
					{
						vec2NumMicroSteps.y -= 1 * speedmodifier;
						if (vec2NumMicroSteps.y < 0)
						{
							vec2NumMicroSteps.y = 0;
							vec2Index.y--;
						}
					}
					// If the new position is not feasible, then revert to old position
					if (CheckPosition(DOWN) == false)
					{
						if (PhaseThroughWall == false)
						{
							vec2Index = vec2OldIndex;
							vec2NumMicroSteps.y = 0;
						}
					}

					//Player is not idle
					idle = false;

					//Set Direction
					Direction = 3;

					// Constraint the player's position within the screen boundary
					Constraint(DOWN);

					//CS: Play the "down" animation
					animatedSprites->PlayAnimation("down", -1, 1.0f);
				}
			}
			else
			{
				if (cKeyboardController->IsKeyPressed(GLFW_KEY_UP))
				{
					if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE)
					{
						cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
						cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 2.5f * jumpheightmodifier));
						iJumpCount += 1;
						cSoundController->PlaySoundByID(12);
					}
					else
					{
						if (iJumpCount < jumpcount)
						{
							cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
							cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 2.0f));
							iJumpCount += 1;
							cSoundController->PlaySoundByID(12);
						}
					}
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_ENTER))
				{
					//Player is not idle
					idle = false;
					if (Direction == 0)
					{
						//CS: Play the "left" animation
						attackAnim = 0.3;
						animatedSprites->PlayAnimation("AttackLeft", -1, attackAnim);
						CSoundController::GetInstance()->PlaySoundByID(15);
						attack = !attack;
					}
					else if (Direction == 1)
					{
						//CS: Play the "right" animation
						attackAnim = 0.3;
						animatedSprites->PlayAnimation("AttackRight", -1, attackAnim);
						CSoundController::GetInstance()->PlaySoundByID(15);
						attack = !attack;
					}
					 

					//CS: Init the color to white
					runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
				}
			}
		}
		else if (attack)
		{
			if (attackAnim <= 0)
				attack = !attack;
			else
				attackAnim -= dElapsedTime;
		}

		static bool Button_Pressed2 = false;
		if (!Button_Pressed2 && (cKeyboardController->IsKeyPressed(GLFW_KEY_LEFT) || cKeyboardController->IsKeyPressed(GLFW_KEY_RIGHT)))
		{
			if (TapCount == 0)
			{
				if (cKeyboardController->IsKeyPressed(GLFW_KEY_LEFT))
				{
					PreviousTapDirection = 263;
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_RIGHT))
				{
					PreviousTapDirection = 262;
				}
			}
			else if (TapCount == 1)
			{
				if (cKeyboardController->IsKeyPressed(GLFW_KEY_LEFT))
				{
					CurrentTapDirection = 263;
				}
				else if (cKeyboardController->IsKeyPressed(GLFW_KEY_RIGHT))
				{
					CurrentTapDirection = 262;
				}
			}
			Button_Pressed2 = true;
			TapCount++;
		}
		else if (Button_Pressed2 && !(cKeyboardController->IsKeyPressed(GLFW_KEY_LEFT) || cKeyboardController->IsKeyPressed(GLFW_KEY_RIGHT)))
		{
			Button_Pressed2 = false;
		}
	}
	
	if (TapCount == 1)
	{
		DoubleTapTimer += dElapsedTime;
	}
	else if (TapCount == 2 && PreviousTapDirection == CurrentTapDirection)
	{
		if (DoubleTapTimer < 0.4f)
		{
			if(Player == 1)
			{
				// Press A
				if (CurrentTapDirection == 65)
				{
					Teleport(LEFT);
				}
				// Press D
				if (CurrentTapDirection == 68)
				{
					Teleport(RIGHT);
				}
			}
			else if (Player == 2)
			{
				// Press Left
				if (CurrentTapDirection == 263)
				{
					Teleport(LEFT);
				}
				// Press Right
				if (CurrentTapDirection == 262)
				{
					Teleport(RIGHT);
				}
			}
		}
		DoubleTapTimer = 0;
		TapCount = 0;
	}
	else if (TapCount == 2)
	{
		PreviousTapDirection = CurrentTapDirection = NULL;
		DoubleTapTimer = 0;
		TapCount = 0;
	}

	if (idle == true)
	{
		if (Player == 1)
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
		else if (Player == 2)
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
	}
	
	OnLadder = false;

	UpdateJumpFall(dElapsedTime);

	jumpheightmodifier = 1;

	if (Timer <= 0)
	{
		speedmodifier = 1;
	}
	else
	{
		Timer -= dElapsedTime;
	}

	if (PhaseTimer <= 0)
	{
		PhaseThroughWall = false;
	}
	else
	{
		PhaseTimer -= dElapsedTime;
	}


	//Player colour based on speed modifier buffs
	{
		if (speedmodifier == 0)
		{
			//FreezeTrap
			runtimeColour = glm::vec4(0.0, 0.0, 1.0, 1.0);
		}
		if (speedmodifier == 0.5)
		{
			//SlowTrap
			runtimeColour = glm::vec4(0.5, 0.5, 0.5, 1.0);
		}
		if (speedmodifier == 2)
		{
			//SpeedBoost
			runtimeColour = glm::vec4(1.0, 0.0, 0.0, 1.0);
		}
	}

	AtExit = false;

	//Interact with Map
	InteractWithMap(0, 0);
	InteractWithMap(0, -1);
	InteractWithMap(1, 0);

	//Check if no lives left
	cInventoryItem = cInventoryManager->GetItem("Lives");
	if (cInventoryItem->GetCount() < 0)
	{
		//Player loses the game
		cGameManager->bPlayerLost = true;
	}

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x*cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y*cSettings->MICRO_STEP_YAXIS);
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CPlayer2D_V2::PreRender(void)
{
	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CPlayer2D_V2::Render(void)
{
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
void CPlayer2D_V2::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

void CPlayer2D_V2::SetTeleportPower(bool newState)
{
	teleportpower = newState;
}

bool CPlayer2D_V2::GetTeleportPower(void)
{
	return teleportpower;
}
unsigned int CPlayer2D_V2::GetJumpCount(void)
{
	return jumpcount;
}

unsigned int CPlayer2D_V2::GetAtExit(void)
{
	return AtExit;
}

void CPlayer2D_V2::SetHealth(unsigned int Health)
{
	this->Health = Health;
}

void CPlayer2D_V2::SetInvulnerabilityFrame(float InvulnerabilityFrame)
{
	this->InvulnerabilityFrame = InvulnerabilityFrame;
}

unsigned int CPlayer2D_V2::GetHealth(void)
{
	return this->Health;
}

float CPlayer2D_V2::GetInvulnerabilityFrame(void)
{
	return this->InvulnerabilityFrame;
}

bool CPlayer2D_V2::Getattack(void)
{
	return attack;
}

unsigned int CPlayer2D_V2::GetDirection(void)
{
	return Direction;
}


void CPlayer2D_V2::SetJumpCount(unsigned int newCount)
{
	jumpcount = newCount;
}

/**
 @brief Constraint the player's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CPlayer2D_V2::Constraint(DIRECTION eDirection)
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
		cout << "CPlayer2D::Constraint: Unknown direction." << endl;
	}
}

void CPlayer2D_V2::InteractWithMap(int xdisplacement, int ydisplacement)
{

	switch (cMap2D->GetMapInfo(vec2Index.y + ydisplacement,vec2Index.x + xdisplacement))
	{
	//Teleport
	case 15:
		if (xdisplacement == 0 && ydisplacement == 0)
		{
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
			teleportpower = true;
		}
		break;
	//PhaseThroughWall
	case 16:
		if (xdisplacement == 0 && ydisplacement == 0)
		{
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
			PhaseThroughWall = true;
			PhaseTimer = 5;
		}
		break;
	//ExtraJump
	case 17:
		if (xdisplacement == 0 && ydisplacement == 0)
		{
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
			jumpcount += 1;
		}
		break;
	//Exit
	case 18:  
		cInventoryItem = cInventoryManager->GetItem("Rune");
		if (cInventoryItem->iItemCount == cInventoryItem->iItemMaxCount && (xdisplacement == 0 && ydisplacement == 0))
		{
			cInventoryItem = cInventoryManager->GetItem("Lives");
			cInventoryItem->iItemCount = cInventoryItem->GetMaxCount();
			Prejumpcount = jumpcount;
			teleportpower = false;
			AtExit = true;
		}
		break;
	//Holy Water
	case 19:
		if (Player == 1 && (xdisplacement == 0 && ydisplacement == 0) && InvulnerabilityFrame <= 0)
		{
			Health -= 1;
			InvulnerabilityFrame += 0.5;
		}
		break;
	//Spikes
	case 20:
		if (Player == 2 && (xdisplacement == 0 && ydisplacement == 0) && InvulnerabilityFrame <= 0)
		{ 
			Health -= 1;
			InvulnerabilityFrame += 0.5;
		}
		break;
	//Lava
	case 21:
		if (xdisplacement == 0 && ydisplacement == 0 && InvulnerabilityFrame <= 0)
		{
			Health -= 1;
			InvulnerabilityFrame += 0.5;
		}
		break;
	//Lives
	case 22:
		if (xdisplacement == 0 && ydisplacement == 0)
		{
			//Erase Lives from this position
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
			cInventoryItem = cInventoryManager->GetItem("Lives");
			Health = 5;
			cInventoryItem->Add(1);
			cSoundController->PlaySoundByID(1);
		}
		break;
	//Ladder
	case 23:
		if (xdisplacement == 0 && ydisplacement == 0)
		{
			//Allow user to move up and down
			OnLadder = true;
		}
		break;
	//Chest
	case 24:
		cInventoryItem = cInventoryManager->GetItem("Key");
		if (cInventoryItem->GetCount() > 0 && (xdisplacement == 0 && ydisplacement == 0))
		{
			cInventoryItem->iItemCount--;
			for (int i = 0; i < cObjectList->size(); i++)
			{
				if (cObjectList->at(i)->vec2Index.x == vec2Index.x && cObjectList->at(i)->vec2Index.y == vec2Index.y)
				{
					cObjectList->at(i)->SetIdle(true);
					cObjectList->at(i)->SetTransition(true);
					cMap2D->SetMapInfo(vec2Index.y+1, vec2Index.x, 27);
					LoadObject = true;
					break;
				}
			}
		}
		break;
	//case 25:
	//	//Erase Coin from this position
	//	cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
	//	cInventoryItem = cInventoryManager->GetItem("Coin");
	//	cInventoryItem->Add(1);
	//	break;
	//Key
	case 26:
		cInventoryItem = cInventoryManager->GetItem("Key");
		if (cInventoryItem->GetCount() == 0 && (xdisplacement == 0 && ydisplacement == 0))
		{
			//Erase Key from this position
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
			//Erase Key from ObjectList
			for (int i = 0; i < cObjectList->size(); i++)
			{
				if (cObjectList->at(i)->vec2Index.x == vec2Index.x && cObjectList->at(i)->vec2Index.y == vec2Index.y)
				{
					cObjectList->erase(cObjectList->begin() + i);
					cInventoryItem->Add(1);
					break;
				}
			}
		}
		break;
	//Rune
	case 27:
		cInventoryItem = cInventoryManager->GetItem("Rune");
		if (cInventoryItem->GetCount() == 0 && (xdisplacement == 0 && ydisplacement == 0))
		{
			//Erase Rune from this position
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
			//Erase Rune from ObjectList
			for (int i = 0; i < cObjectList->size(); i++)
			{
				if (cObjectList->at(i)->vec2Index.x == vec2Index.x && cObjectList->at(i)->vec2Index.y == vec2Index.y)
				{
					cObjectList->erase(cObjectList->begin() + i);
					cInventoryItem->Add(1);
					break;
				}
			}
			cSoundController->PlaySoundByID(1);
		}
		break;
	//Door
	case 28:
		cInventoryItem = cInventoryManager->GetItem("Key"); 
		CObject2D* DoorPosition;
		for (int i = 0; i < cObjectList->size(); i++)
		{
			if (cObjectList->at(i)->vec2Index.x == vec2Index.x + xdisplacement && cObjectList->at(i)->vec2Index.y == vec2Index.y + ydisplacement)
			{
				DoorPosition = cObjectList->at(i);
			}
		}
		//Unlock door and remove key
		if (cInventoryItem->GetCount() > 0 && ((xdisplacement == 1 && ydisplacement == 0) || (xdisplacement == 0 && ydisplacement == 0)))
		{
			cInventoryItem->iItemCount--;
			DoorPosition->SetIdle(true);
			DoorPosition->SetTransition(true);
			LoadObject = true;
			break;
			
		}
		
		
		if (Direction == RIGHT && (xdisplacement == 1 && ydisplacement == 0) && DoorPosition->GetIdle() == false)
		{
			this->vec2Index = vec2OldIndex;
			this->vec2NumMicroSteps.x = 0;
		}
		else if (Direction == LEFT && (xdisplacement == 0 && ydisplacement == 0) && DoorPosition->GetIdle() == false)
		{
			this->vec2NumMicroSteps.x = cSettings->NUM_STEPS_PER_TILE_XAXIS; 
		}
		break;

	//SpeedBoost
	case 104:
		if (xdisplacement == 0 && ydisplacement == -1)
		{
			speedmodifier = 2;
			Timer = 5;
		}
		break;
	//SlowTrap
	case 101:
		if (xdisplacement == 0 && ydisplacement == -1)
		{
			speedmodifier = 0.5;
			Timer = 5;
		}
		break;
	//JumpBoost
	case 102:
		if (xdisplacement == 0 && ydisplacement == -1)
		{
			jumpheightmodifier = 1.5;
			runtimeColour = glm::vec4(0.0, 1.0, 0.0, 1.0);
		}
		break;
	//FreezeTrap
	case 103:
		if (xdisplacement == 0 && ydisplacement == -1)
		{
			speedmodifier = 0;
			Timer = 5;
			//Erase Freeze Trap from this position and replace with normal tile
			cMap2D->SetMapInfo(vec2Index.y-1, vec2Index.x, 100);

		}
		break;
	default:
		break;
	}
}

bool CPlayer2D_V2::CheckPosition(DIRECTION eDirection)
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
			if ((vec2NumMicroSteps.x != 0) && (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (vec2NumMicroSteps.y != 0)
		{
			if (vec2NumMicroSteps.x != 0)
			{
				// If the 2 grids are not accessible, then return false
				if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100) ||
					(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x + 1) >= 100))
				{
					return false;
				}
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
			if ((vec2NumMicroSteps.y != 0) && (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100))
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (vec2NumMicroSteps.x != 0)
		{
			if (vec2NumMicroSteps.y != 0) {
				// If the 2 grids are not accessible, then return false
				if ((cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100) ||
					(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x + 1) >= 100))
				{
					return false;
				}
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
		cout << "CPlayer2D::CheckPosition: Unknown direction." << endl;
	}

	return true;
}

void CPlayer2D_V2::Teleport(DIRECTION direction)
{
	glm::vec2 Tempvec2Index;
	Tempvec2Index = this->vec2Index;
	if (teleportpower == true)
	{
		if (direction == RIGHT)
		{
			Tempvec2Index.x += 5;
			vec2Index = Tempvec2Index;
		}
		else if (direction == LEFT)
		{
			Tempvec2Index.x -= 5;
			vec2Index = Tempvec2Index;
		}
	}
}

// Update Jump or Fall
void CPlayer2D_V2::UpdateJumpFall(const double dElapsedTime)
{
	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP)
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.SetTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();

		// Store the current vec2Index.y
		int iIndex_YAxis_OLD = vec2Index.y;

		int iDisplacement_MicroSteps = (int)(v2Displacement.y / cSettings->MICRO_STEP_YAXIS); //Displacement divide by distance for 1 microstep
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y += iDisplacement_MicroSteps;
			if (vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				vec2NumMicroSteps.y -= cSettings->NUM_STEPS_PER_TILE_YAXIS;
				if (vec2NumMicroSteps.y < 0)
					vec2NumMicroSteps.y = 0;
				vec2Index.y++;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(UP);

		// Iterate through all rows until the proposed row
		// Check if the player will hit a tile; stop jump if so.
		int iIndex_YAxis_Proposed = vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i <= iIndex_YAxis_Proposed; i++)
		{
			// Change the player's index to the current i value
			vec2Index.y = i;
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(UP) == false)
			{
				// Align with the row
				vec2NumMicroSteps.y = 0;
				// Set the Physics to fall status
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				break;
			}
		}

		// If the player is still jumping and the initial velocity has reached zero or below zero, 
		// then it has reach the peak of its jump
		if ((cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP) && (cPhysics2D.GetDisplacement().y <= 0.0f))
		{
			// Set status to fall
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}
	}
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL)
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.SetTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();

		// Store the current vec2Index.y
		int iIndex_YAxis_OLD = vec2Index.y;

		// Translate the displacement from pixels to indices
		int iDisplacement_MicroSteps = (int)(v2Displacement.y / cSettings->MICRO_STEP_YAXIS);

		if (vec2Index.y >= 0)
		{
			vec2NumMicroSteps.y -= fabs(iDisplacement_MicroSteps);
			if (vec2NumMicroSteps.y < 0)
			{
				vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				vec2Index.y--;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(DOWN);

		// Iterate through all rows until the proposed row
		// Check if the player will hit a tile; stop fall if so.
		int iIndex_YAxis_Proposed = vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
		{
			// Change the player's index to the current i value
			vec2Index.y = i;
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(DOWN) == false)
			{
				// Revert to the previous position
				if (i != iIndex_YAxis_OLD)
					vec2Index.y = i + 1;
				// Set the Physics to idle status
				cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
				cSoundController->PlaySoundByID(13);
				iJumpCount = 0;
				vec2NumMicroSteps.y = 0;
				break;
			}
		}
	}
}
bool CPlayer2D_V2::IsMidAir(void)
{
	//if the player is at the bottom row, then he is not in mid-air for sure
	if (vec2Index.y == 0)
		return false;

	//Check if the tile below the player's current positon is empty
	if ((vec2NumMicroSteps.x == 0) && (cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x) < 100 && cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x) != 23))
	{
		return true;
	}
	return false;
}

