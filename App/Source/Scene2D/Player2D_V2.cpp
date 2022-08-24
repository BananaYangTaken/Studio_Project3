/**
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
#include "..\Application.h"
#include "..\Scene2D\EnemyBase.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D_V2::CPlayer2D_V2(void)
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	, cMouseController(NULL)
	, animatedSprites(NULL)
	, runtimeColour(glm::vec4(1.0f))
	, Direction(DOWN)
	, cObjectList(NULL)
	, Health(NULL)
	, MaxHealth(NULL)
	, InvulnerabilityFrame(NULL)
	, LButtonState(NULL)
	, Speed(NULL)
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


/**
  @brief Initialise this instance
  */
void CPlayer2D_V2::SetObjectList(std::vector<CObject2D*>* newObjectList)
{
	cObjectList = newObjectList;
}
void CPlayer2D_V2::SetEnemyList(std::vector<CEntity2D*>* newEnemyList)
{
	cEnemyList = newEnemyList;
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
	// Store the mouse controller singleton instance here
	cMouseController = CMouseController::GetInstance();

	// Load the sounds into CSoundController
	cSoundController = CSoundController::GetInstance();

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the C32D instance
	cMap2D = CMap2D::GetInstance();

	// Get the handler to the CGUI_Scene2D instance
	cGUI_Scene2D = CGUI_Scene2D::GetInstance();

	// Get the handler to the CInventoryManager instance
	cInventoryManager = CInventoryManager::GetInstance();

	//Get the handler to the Game Manager Instance
	cGameManager = CGameManager::GetInstance();
	//Get the handler to the Game Manager Instance
	cProjectileManager = CProjectileManager::GetInstance();

	LoadObject = false;

	Health = MaxHealth = 100;
	InvulnerabilityFrame = 0;

	Rotation = 0;

	Speed = 1;

	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow;
	unsigned int uiCol;
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


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Player_Sprites.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Player_Sprites.png" << endl;
		return false;
	}

	// Create the quad mesh for the player
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(13, 20, (cSettings->TILE_WIDTH + cSettings->TILE_HEIGHT) * 0.5, (cSettings->TILE_WIDTH + cSettings->TILE_HEIGHT) * 0.5);
	animatedSprites->AddAnimation("meleeIdle", 0, 19);
	animatedSprites->AddAnimation("meleeAttack", 20, 34);
	animatedSprites->AddAnimation("meleeMove", 40, 59);

	animatedSprites->AddAnimation("pistolIdle", 60, 79);
	animatedSprites->AddAnimation("pistolMove", 80, 99);
	animatedSprites->AddAnimation("pistolAttack", 100, 114);
	animatedSprites->AddAnimation("pistolReload", 120, 125);
	animatedSprites->AddAnimation("pistolFire", 140, 142);

	animatedSprites->AddAnimation("rifleIdle", 160, 179);
	animatedSprites->AddAnimation("rifleAttack", 180, 194);
	animatedSprites->AddAnimation("rifleMove", 200, 219);
	animatedSprites->AddAnimation("rifleReload", 220, 234);
	animatedSprites->AddAnimation("rifleFire", 240, 242);

	AnimationTimer = 0;


	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();
	
	//Load Inventory Item
	{
		// Add a Health as one of the inventory items
		cInventoryItem = cInventoryManager->Add("Health", "Image/Health.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Dawn", "Image/GUI/dawn.png", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Morning", "Image/GUI/morning.png", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Midday", "Image/GUI/midday.png", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Afternoon", "Image/GUI/afternoon.png", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Evening", "Image/GUI/evening.png", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Dusk", "Image/GUI/dusk.png", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Midnight", "Image/GUI/midnight.png", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Afternight", "Image/GUI/afternight.png", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("bloodmoon", "Image/GUI/Blood Moon.png", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("solareclipse", "Image/GUI/Solar Eclipse.png", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Hard wood", "Image/Iteme/Hard wood.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Stone Ore", "Image/Iteme/Stone Ore.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Scrap Metal", "Image/Iteme/Scrap Metal.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Pistol Bullets", "Image/Iteme/Pistol Bullets.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Rifle Bullets", "Image/Iteme/Rifle Bullets.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Rifle", "Image/Iteme/Rifle.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Pistol", "Image/Iteme/Pistol.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Medkit", "Image/Iteme/Medkit.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Bandage", "Image/Iteme/Bandage.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Fabric", "Image/Iteme/Fabric.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Yellow Keycard", "Image/Iteme/Yellow Keycard.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Flashlight", "Image/Iteme/Flashlight.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("MuzzleDevice", "Image/Iteme/muzzleDevice.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Extended Magazine", "Image/Iteme/extendedmag.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);

		cInventoryItem = cInventoryManager->Add("Blueprint", "Image/Iteme/Blueprint.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
		cInventoryItem = cInventoryManager->Add("empty0", "Image/BlankBox.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
		cInventoryItem = cInventoryManager->Add("empty1", "Image/BlankBox.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
		cInventoryItem = cInventoryManager->Add("empty2", "Image/BlankBox.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
		cInventoryItem = cInventoryManager->Add("empty3", "Image/BlankBox.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
		cInventoryItem = cInventoryManager->Add("empty4", "Image/BlankBox.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
		cInventoryItem = cInventoryManager->Add("empty5", "Image/BlankBox.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
		cInventoryItem = cInventoryManager->Add("empty6", "Image/BlankBox.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
		cInventoryItem = cInventoryManager->Add("empty7", "Image/BlankBox.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
		cInventoryItem = cInventoryManager->Add("empty8", "Image/BlankBox.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
		cInventoryItem = cInventoryManager->Add("empty9", "Image/BlankBox.tga", 1, 0);
		cInventoryItem->vec2Size = glm::vec2(25, 25);
	}
	LButtonState = false;

	return true;
}

/**
 @brief Reset this instance
 */
bool CPlayer2D_V2::Reset()
{
	unsigned int uiRow, uiCol;
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
	InvulnerabilityFrame = 0;
	Health = 5;

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
	
	// Reset Rune
	cInventoryItem = cInventoryManager->GetItem("Rune");
	cInventoryItem->iItemCount = 0;
	// Reset Key
	cInventoryItem = cInventoryManager->GetItem("Key");
	cInventoryItem->iItemCount = 0;

	//CS: Init the color to white
	//runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	return true;
}

/**
 @brief Update this instance
 */
void CPlayer2D_V2::Update(const double dElapsedTime)
{
	std::cout << vec2Index.x << " " << vec2Index.y << std::endl;
	motion = false;
	// Store the old position
	vec2OldIndex = vec2Index;
	if (cGUI_Scene2D->healing == true)
	{
		cGUI_Scene2D->actiontext2 = "Using...";

		cout << "USING!" << endl;
		healtimer++;
		if (healtimer >= cGUI_Scene2D->healtime)
		{
			cGUI_Scene2D->DecreaseInventoryItemCount(cGUI_Scene2D->inventory_item_name_list[cGUI_Scene2D->hotbarselection - 1], 1);
			Health = 100;
			cGUI_Scene2D->healing = false;
			cGUI_Scene2D->actiontext2 = "...";
			healtimer = 0;
		}
	}

	CInventoryItem HeldItem = *(cInventoryManager->GetItem(cGUI_Scene2D->GetCurrentHotbarItem()));
	if (cGUI_Scene2D->GetCurrentHotbarItem() == "Rifle" || cGUI_Scene2D->GetCurrentHotbarItem() == "Pistol")
	{
		if(HeldItem.WData->ReloadTimer > 0)
			cGUI_Scene2D->actiontext2 = HeldItem.sName + ": RELOADING...";
		else
			cGUI_Scene2D->actiontext2 = HeldItem.sName + ":"+ std::to_string(HeldItem.GData->CurrentAmmoSize) +"/"+ std::to_string(HeldItem.GData->MaxAmmoSize);
	}
	
	// Get keyboard updates
	if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
	{
		// Calculate the new position to the left
		if (vec2Index.x >= 0)
		{
			vec2NumMicroSteps.x -= Speed;
			if (vec2NumMicroSteps.x < 0)
			{
				vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
				vec2Index.x--;
				cSoundController->PlaySoundByID(36);
			}
		}
		// Constraint the player's position within the screen boundary
		Constraint(LEFT);
		//If the new position is not feasible, then revert to old position
		if (CheckPosition(LEFT) == false)
		{
			vec2Index.x = vec2OldIndex.x;
			vec2NumMicroSteps.x = 0;
		}
		motion = true;
		//Set Direction
		Direction = 0;
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
	{
		// Calculate the new position to the right
		if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			vec2NumMicroSteps.x += Speed;

			if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
			{
				vec2NumMicroSteps.x = 0;
				vec2Index.x++;
				cSoundController->PlaySoundByID(36);
			}
		}
		// Constraint the player's position within the screen boundary
		Constraint(RIGHT);
		//If the new position is not feasible, then revert to old position
		if (CheckPosition(RIGHT) == false)
		{
			//vec2Index.x = vec2OldIndex.x;
			vec2NumMicroSteps.x = 0;
		}
		motion = true;
		//Set Direction
		Direction = 1;
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
	{
		// Calculate the new position up
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y += Speed;
			if (vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				vec2NumMicroSteps.y = 0;
				vec2Index.y++;
				cSoundController->PlaySoundByID(36);
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(UP);
		// If the new position is not feasible, then revert to old position
		if (CheckPosition(UP) == false)
		{
			vec2NumMicroSteps.y = 0;
		}
		motion = true;
		//Set Direction
		Direction = 2;
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_S))
	{
		// Calculate the new position down
		if (vec2Index.y >= 0)
		{
			vec2NumMicroSteps.y -= Speed;
			if (vec2NumMicroSteps.y < 0)
			{
				vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				vec2Index.y--;
				cSoundController->PlaySoundByID(36);
			}
		}
		// Constraint the player's position within the screen boundary
		Constraint(DOWN);
		// If the new position is not feasible, then revert to old position
		if (CheckPosition(DOWN) == false)
		{
			vec2Index.y = vec2OldIndex.y;
			vec2NumMicroSteps.y = 0;
		}

		motion = true;
		//Set Direction
		Direction = 3;
	}

	//Player Use Item
	if (cGUI_Scene2D->checkinginventory == false && cGUI_Scene2D->crafting == false && cGUI_Scene2D->chestactive == false)
	{
		//Shooting/Knifing
		if (cMouseController->IsButtonDown(0) && !LButtonState)
		{
			LButtonState = true;
			//Melee
			if (HeldItem.itemtype == ITEM_TYPE::RESOURCES || HeldItem.itemtype == ITEM_TYPE::MELEE)
			{
				//Attack with Knife
				AnimationTimer = 0.3f;
				animatedSprites->PlayAnimation("meleeAttack", -1, AnimationTimer);
				cSoundController->PlaySoundByID(15);
				cProjectileManager->SpawnProjectile(vec2Index, vec2NumMicroSteps, Rotation, 0, CProjectile2D::FRIENDLY, CProjectile2D::KNIFE, 30);
			}
			else if (HeldItem.itemtype == ITEM_TYPE::GUN
				&& HeldItem.GData->firingtype == FIRING_TYPE::SEMIAUTO
				&& HeldItem.GData->CurrentAmmoSize > 0
				&& HeldItem.GData->FiringCooldown == 0
				&& HeldItem.WData->ReloadTimer == 0)
			{
				//Attack with Semi Auto Gun
				AnimationTimer = 0.1f;
				animatedSprites->PlayAnimation("pistolFire", -1, AnimationTimer);
				cSoundController->PlaySoundByID(23);
				cProjectileManager->SpawnProjectile(vec2Index, vec2NumMicroSteps, Rotation, 400, CProjectile2D::FRIENDLY, CProjectile2D::BULLET, HeldItem.WData->Damage);
				--HeldItem.GData->CurrentAmmoSize;
				HeldItem.GData->FiringCooldown = HeldItem.GData->FireRate;
			}
		}
		else if (cMouseController->IsButtonUp(0) && LButtonState)
		{
			LButtonState = false;
		}
		if (LButtonState)
		{
			if (HeldItem.itemtype == ITEM_TYPE::GUN 
				&& HeldItem.GData->firingtype == FIRING_TYPE::FULLAUTO 
				&& HeldItem.GData->CurrentAmmoSize > 0 
				&& HeldItem.GData->FiringCooldown == 0
				&& HeldItem.WData->ReloadTimer == 0)
			{
				//Attack with Full Auto Gun
				AnimationTimer = 0.1f;
				animatedSprites->PlayAnimation("rifleFire", -1, AnimationTimer);
				cSoundController->PlaySoundByID(19);
				cProjectileManager->SpawnProjectile(vec2Index, vec2NumMicroSteps, Rotation, 400, CProjectile2D::FRIENDLY, CProjectile2D::BULLET, HeldItem.WData->Damage);
				--HeldItem.GData->CurrentAmmoSize;
				HeldItem.GData->FiringCooldown = HeldItem.GData->FireRate;
			}
		}
		if(HeldItem.itemtype == ITEM_TYPE::GUN)
		{ 
			//Reload
			if (cKeyboardController->IsKeyPressed(GLFW_KEY_R) && AnimationTimer == 0)
			{
	
				if (HeldItem.GData->CurrentAmmoSize < HeldItem.GData->MaxAmmoSize)
				{
					int bulletdifference = HeldItem.GData->MaxAmmoSize; -HeldItem.GData->CurrentAmmoSize;
					cout << "BULLET DIFF: " + bulletdifference << endl;
					HeldItem.WData->ReloadTimer = HeldItem.WData->ReloadTime;
					AnimationTimer = HeldItem.WData->ReloadTime;
				
					if (HeldItem.GData->firingtype == FIRING_TYPE::FULLAUTO && cGUI_Scene2D->checkifItemExists("Rifle Bullets") >= 30)
					{
						animatedSprites->PlayAnimation("rifleReload",-1, HeldItem.WData->ReloadTime);
						cSoundController->PlaySoundByID(25);
						cGUI_Scene2D->DecreaseInventoryItemCount("Rifle Bullets", bulletdifference);
						HeldItem.GData->CurrentAmmoSize = HeldItem.GData->MaxAmmoSize;
					}
					else if (HeldItem.GData->firingtype == FIRING_TYPE::SEMIAUTO && cGUI_Scene2D->checkifItemExists("Pistol Bullets") >= 17)
					{
						animatedSprites->PlayAnimation("pistolReload",-1, HeldItem.WData->ReloadTime);
						cSoundController->PlaySoundByID(24);
						cGUI_Scene2D->DecreaseInventoryItemCount("Pistol Bullets", 17);
						HeldItem.GData->CurrentAmmoSize = HeldItem.GData->MaxAmmoSize;
					}
				}
			}
			//Cooldowns 
			if (HeldItem.GData->FiringCooldown > 0)
			{
				HeldItem.GData->FiringCooldown -= dElapsedTime;
			}
			else
			{
				HeldItem.GData->FiringCooldown = 0;
			}
			if (HeldItem.WData->ReloadTimer > 0)
			{
				HeldItem.WData->ReloadTimer -= dElapsedTime;
			}
			else
			{
				HeldItem.WData->ReloadTimer = 0;
			}
		}
	}
	if (InvulnerabilityFrame > 0)
	{
		InvulnerabilityFrame -= dElapsedTime;
	}
	else
	{
		InvulnerabilityFrame = 0;
	}
	if (AnimationTimer > 0)
	{
		AnimationTimer -= dElapsedTime;
	}
	else
	{
		AnimationTimer = 0;
	}
	if (Health <= 0 && InvulnerabilityFrame <= 0)
	{
		CGameManager::GetInstance()->bPlayerLost = true;
		Health = 0;
	}
	//Animation
	if (AnimationTimer == 0)
	{
		if (motion == false)
		{
			if (HeldItem.itemtype == ITEM_TYPE::GUN && HeldItem.WData->ReloadTimer == 0)
			{
				if (HeldItem.GData->firingtype == FIRING_TYPE::SEMIAUTO)
				{
					animatedSprites->PlayAnimation("pistolIdle", -1, 10.0f);
				}
				else if (HeldItem.GData->firingtype == FIRING_TYPE::FULLAUTO)
				{
					animatedSprites->PlayAnimation("rifleIdle", -1, 10.0f);
				}
			}
			else
			{
				animatedSprites->PlayAnimation("meleeIdle", -1, 10.0f);
			}
		}
		else if (motion == true)
		{
			if (HeldItem.itemtype == ITEM_TYPE::GUN && HeldItem.WData->ReloadTimer == 0)
			{
				if (HeldItem.GData->firingtype == FIRING_TYPE::SEMIAUTO)
				{
					animatedSprites->PlayAnimation("pistolMove", -1, 10.0f);
				}
				else if (HeldItem.GData->firingtype == FIRING_TYPE::FULLAUTO)
				{
					animatedSprites->PlayAnimation("rifleMove", -1, 10.0f);
				}
			}
			else
			{
				animatedSprites->PlayAnimation("meleeMove", -1, 1.0f);
			}
		}
	}

	
	//Interact with Map
	
	InteractWithMap(0,0);
	InteractWithMap(0, 1);
	InteractWithMap(0, -1);
	InteractWithMap(1, 0);
	InteractWithMap(-1, 0);
	InteractWithMap(1,1);
	InteractWithMap(-1, -1);
	InteractWithMap(-1, 1);
	InteractWithMap(1, -1);
	

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);
	//Calculate Position of Entity on Screen
	glm::vec2 ScreenPos = glm::vec2(0, 0);
	// Update the UV Coordinates
	{
		unsigned int AtBorderY = 0; // Top 1, Bottom 2,
		unsigned int AtBorderX = 0; // Left 1, Right 2
		//Check if Map View/Camera at Borders
		if (vec2Index.x > (cSettings->NUM_TILES_XAXIS - (cSettings->VIEW_TILES_XAXIS * 0.5) - 1)) //Right Side Border
		{
			ScreenPos.x = cSettings->VIEW_TILES_XAXIS - (cSettings->NUM_TILES_XAXIS - vec2Index.x);
			AtBorderX = 2;
		}
		else if (vec2Index.x < cSettings->VIEW_TILES_XAXIS * 0.5 ) // Left Side Border
		{
			ScreenPos.x = vec2Index.x;
			AtBorderX = 1;
		}

		if (vec2Index.y > (cSettings->NUM_TILES_YAXIS - (cSettings->VIEW_TILES_YAXIS * 0.5) - 1)) //Top Side Border
		{
			ScreenPos.y = cSettings->VIEW_TILES_YAXIS - (cSettings->NUM_TILES_YAXIS - vec2Index.y);
			AtBorderY = 1;
		}
		else if (vec2Index.y < cSettings->VIEW_TILES_YAXIS * 0.5) //Bottom Side Border
		{
			ScreenPos.y = vec2Index.y;
			AtBorderY = 2;
		}

		//If not at Border, Entity at Center of Screen displaced by x:1 y:1
		if (ScreenPos.x == 0 && AtBorderX == 0)
		{
			ScreenPos.x = cSettings->VIEW_TILES_XAXIS * 0.5;
		}
		if (ScreenPos.y == 0 && AtBorderY == 0)
		{
			ScreenPos.y = cSettings->VIEW_TILES_YAXIS * 0.5;
		}

		//Convert position to UV Coords
		if (AtBorderX == 0)
		{
			vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, ScreenPos.x, false, 0);
		}
		else if (AtBorderX == 1)
		{
			vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, ScreenPos.x, false, cSettings->MICRO_STEP_XAXIS * vec2NumMicroSteps.x);
		}
		else if (AtBorderX == 2)
		{
			vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, ScreenPos.x, false, cSettings->MICRO_STEP_XAXIS * vec2NumMicroSteps.x);
		}
		if (AtBorderY == 0)
		{
			vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, ScreenPos.y, false, 0);
		}
		else if (AtBorderY == 1)
		{
			vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, ScreenPos.y, false, cSettings->MICRO_STEP_YAXIS * vec2NumMicroSteps.y);
		}
		else if (AtBorderY == 2)
		{
			vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, ScreenPos.y, false, cSettings->MICRO_STEP_YAXIS * vec2NumMicroSteps.y);
		}
	}
	//Update Rotation
	{
		//Mouse Position
		double x, y;
		Application::GetCursorPos(&x, &y);
		float mousexpos = ((x / cSettings->iWindowWidth) * cSettings->VIEW_TILES_XAXIS) - cSettings->VIEW_TILES_XAXIS * 0.5;
		float mouseypos = (abs((y - cSettings->iWindowHeight) / cSettings->iWindowHeight) * cSettings->VIEW_TILES_YAXIS) - cSettings->VIEW_TILES_YAXIS * 0.5;\
		//Calculate Origin
		glm::vec2 Origin = ScreenPos - glm::vec2(cSettings->VIEW_TILES_XAXIS * 0.5 + 1, cSettings->VIEW_TILES_YAXIS * 0.5 + 1);
		//Calculate Rotation
		Rotation = cPhysics2D.CalculateRotation(Origin , glm::vec2(1, 0), glm::vec2(mousexpos, mouseypos));
		if (vec2Index.x > (cSettings->NUM_TILES_XAXIS - (cSettings->VIEW_TILES_XAXIS * 0.5) + 2)) //Right Side Border 
		{
			Rotation += Math::PI;
		}
	}
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
	transform = glm::rotate(transform, Rotation, glm::vec3(0,0,1) );
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

void CPlayer2D_V2::SetHealth(float Health)
{
	this->Health = Health;
}

void CPlayer2D_V2::SetInvulnerabilityFrame(float InvulnerabilityFrame)
{
	this->InvulnerabilityFrame = InvulnerabilityFrame;
}

float CPlayer2D_V2::GetHealth(void)
{
	return this->Health;
}
float CPlayer2D_V2::GetMaxHealth(void)
{
	return this->MaxHealth;
}

float CPlayer2D_V2::GetInvulnerabilityFrame(void)
{
	return this->InvulnerabilityFrame;
}


unsigned int CPlayer2D_V2::GetDirection(void)
{
	return Direction;
}

void CPlayer2D_V2::SetMotionFlag(const bool bMotion)
{
	this->motion = bMotion;
}

bool CPlayer2D_V2::GetMotionFlag(void)
{
	return motion;
}

bool CPlayer2D_V2::PlayerChangedPos(void)
{
	if (vec2Index != vec2OldIndex)
	{
		return true;
	}
	return false;
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
	unsigned int IRow;
	unsigned int ICol;
	if ((vec2Index.x + xdisplacement) >= 0 && (vec2Index.x + xdisplacement) <= cSettings->NUM_TILES_XAXIS && (vec2Index.y + ydisplacement) >= 0 && (vec2Index.y + ydisplacement + 1) <= cSettings->NUM_TILES_YAXIS)
	{
		switch (cMap2D->GetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement))
		{
		case 110:
			cGUI_Scene2D->actiontext = "Press F to search crate";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				cout << "CASE 110 " << endl;
				if (cGUI_Scene2D->looting == false)
				{
					cGUI_Scene2D->searchtxt = "SEARCHING...";
					cGUI_Scene2D->issearched = false;
					cGUI_Scene2D->searchtimer = 0;
					cSoundController->PlaySoundByID(18);
					cGUI_Scene2D->checkinginventory = true;
					cGUI_Scene2D->OpenCrate("Large");
					cGUI_Scene2D->looting = true;
					cGUI_Scene2D->crafting = false;
				}
				else
				{
					cout << "get rid of box";
					cGUI_Scene2D->looting = false;
					cGUI_Scene2D->checkinginventory = false;
					cGUI_Scene2D->crafting = false;
					if (cMap2D->GetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement) == 110)
						cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);

				}
			}
			break;
		case 104:
			if (cKeyboardController->IsKeyPressed('F'))
			{
				cout << "CASE 114 " << endl;
				if (cGUI_Scene2D->checkinginventory == false)
				{
					cGUI_Scene2D->checkinginventory = true;
					cGUI_Scene2D->chestactive = true;
					cGUI_Scene2D->crafting = false;
				}
				else
				{
					cGUI_Scene2D->checkinginventory = false;
					cGUI_Scene2D->chestactive = false;
					cGUI_Scene2D->crafting = false;
				}

			}
			break;
			/*	30 - bandages
				32 - Blueprint
				35 - medkit
				36 - pistol bullets
				37 - pistol
				38 - rifle bullets
				39 - rifle


				40 - Scrap
				41 - Stone ore
				33 - Fabric
				34 - hard wood*/

		case 103:
			cGUI_Scene2D->actiontext = "Press F to open crafting menu";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				cout << "CASE 113 " << endl;
				if (cGUI_Scene2D->crafting == false)
				{
					cGUI_Scene2D->crafting = true;
					cGUI_Scene2D->checkinginventory = true;
				}
				else
				{
					cGUI_Scene2D->crafting = false;
					cGUI_Scene2D->checkinginventory = false;
				}
			}
			break;
		case 30:
			cGUI_Scene2D->actiontext = "Press F to pickup bandages";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				if (cGUI_Scene2D->IncreaseInventoryItemCount("Bandage", 1) == 1)
				{
					cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);
				}
				else
				{
					cGUI_Scene2D->actiontext = "Inventory full!";
				}
				cSoundController->PlaySoundByID(22);

			}
			break;
		case 32:
			cGUI_Scene2D->actiontext = "Press F to pickup blueprints";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				if (cGUI_Scene2D->IncreaseInventoryItemCount("Blueprint", 1) == 1)
				{
					cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);
				}
				else
				{
					cGUI_Scene2D->actiontext = "Inventory full!";
				}
				cSoundController->PlaySoundByID(22);

			}
			break;
		case 33:
			cGUI_Scene2D->actiontext = "Press F to pickup fabric";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				if (cGUI_Scene2D->IncreaseInventoryItemCount("Fabric", 1) == 1)
				{
					cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);
				}
				else
				{
					cGUI_Scene2D->actiontext = "Inventory full!";
				}
				cSoundController->PlaySoundByID(22);

			}
			break;
		case 34:
			cGUI_Scene2D->actiontext = "Press F to pickup wood";
			if (cKeyboardController->IsKeyPressed('F'))
			{

				if (cGUI_Scene2D->IncreaseInventoryItemCount("Hard wood", 1) == 1)
				{
					cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);
				}
				else
				{
					cGUI_Scene2D->actiontext = "Inventory full!";
				}
				cSoundController->PlaySoundByID(22);

			}
			break;
		case 35:
			cGUI_Scene2D->actiontext = "Press F to pickup medkit";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				if (cGUI_Scene2D->IncreaseInventoryItemCount("Medkit", 1) == 1)
				{
					cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);
				}
				else
				{
					cGUI_Scene2D->actiontext = "Inventory full!";
				}
				cSoundController->PlaySoundByID(22);

			}
			break;
		case 36:
			cGUI_Scene2D->actiontext = "Press F to pickup pistol bullets";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				if (cGUI_Scene2D->IncreaseInventoryItemCount("Pistol Bullet", 5) == 1)
				{
					cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);
				}
				else
				{
					cGUI_Scene2D->actiontext = "Inventory full!";
				}
				cSoundController->PlaySoundByID(22);

			}
			break;
		case 37:
			cGUI_Scene2D->actiontext = "Press F to pickup pistol";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				if (cGUI_Scene2D->IncreaseInventoryItemCount("Pistol", 1) == 1)
				{
					cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);
				}
				else
				{
					cGUI_Scene2D->actiontext = "Inventory full!";
				}
				cSoundController->PlaySoundByID(22);

			}
			break;
		case 38:
			cGUI_Scene2D->actiontext = "Press F to pickup rifle bullets";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				if (cGUI_Scene2D->IncreaseInventoryItemCount("Rifle Bullet", 1) == 1)
				{
					cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);
				}
				else
				{
					cGUI_Scene2D->actiontext = "Inventory full!";
				}
				cSoundController->PlaySoundByID(22);

			}
			break;
		case 39:
			cGUI_Scene2D->actiontext = "Press F to pickup rifle";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				if (cGUI_Scene2D->IncreaseInventoryItemCount("Rifle", 1) == 1)
				{
					cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);
				}
				else
				{
					cGUI_Scene2D->actiontext = "Inventory full!";
				}
				cSoundController->PlaySoundByID(22);

			}
			break;
		case 40:
			cGUI_Scene2D->actiontext = "Press F to pickup scrap metal";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				if (cGUI_Scene2D->IncreaseInventoryItemCount("Scrap Metal", 1) == 1)
				{
					cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);
				}
				else
				{
					cGUI_Scene2D->actiontext = "Inventory full!";
				}
				cSoundController->PlaySoundByID(22);

			}
			break;
		case 41:
			cGUI_Scene2D->actiontext = "Press F to pickup stone";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				if (cGUI_Scene2D->IncreaseInventoryItemCount("Stone Ore", 1) == 1)
				{
					cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);
				}
				else
				{
					cGUI_Scene2D->actiontext = "Inventory full!";
				}
				cSoundController->PlaySoundByID(22);

			}
			break;
		case 42:
			cGUI_Scene2D->actiontext = "Press F to pickup Yellow Card";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				if (cGUI_Scene2D->IncreaseInventoryItemCount("Yellow Keycard", 1) == 1)
				{
					cMap2D->SetMapInfo(vec2Index.y + ydisplacement, vec2Index.x + xdisplacement, 0);
				}
				else
				{
					cGUI_Scene2D->actiontext = "Inventory full!";
				}
				cSoundController->PlaySoundByID(22);

			}
			break;
		case 115:
			cGUI_Scene2D->actiontext = "Press F to unlock door";
			if (cKeyboardController->IsKeyPressed('F'))
			{
				cout << "HOTBARITEM: " + (cGUI_Scene2D->GetCurrentHotbarItem()) << endl;
				if (cGUI_Scene2D->GetCurrentHotbarItem() == "Yellow Keycard")
				{
					cout << "HOTBARLEVEL: " + cGUI_Scene2D->hotbarlevel << endl;
					unlockedYellow = true;
					cMap2D->SetMapInfo(80, 71, 0);
					cMap2D->SetMapInfo(80, 72, 0);
					cSoundController->PlaySoundByID(37);
					break;
				}

				if (unlockedYellow == false)
				{
					cSoundController->PlaySoundByID(38);
				}
			}
			break;
		default:
			cGUI_Scene2D->actiontext = "...";
			break;
		}
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

