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

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D_V2::CPlayer2D_V2(void)
	: cMap2D(NULL)
	, Player(NULL)
	, cKeyboardController(NULL)
	, animatedSprites(NULL)
	, runtimeColour(glm::vec4(1.0f))
	, Direction(DOWN)
	, cObjectList(NULL)
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


/**
  @brief Initialise this instance
  */
void CPlayer2D_V2::SetObjectList(std::vector<CObject2D*>* newObjectList)
{
	cObjectList = newObjectList;
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
	
	LoadObject = false;

	Health = 5;
	InvulnerabilityFrame = 0;

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

	
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/SkeletonArcher_SpriteSheet.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/SkeletonArcher_SpriteSheet.png" << endl;
		return false;
	}

	// Create the quad mesh for the player
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

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
	animatedSprites->AddAnimation("AttackRight", 65, 74);

	attackAnim = 0;


	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();

	// Add a Lives as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Lives", "Image/Scene2D_Lives.tga", 3, 3);
	cInventoryItem->vec2Size = glm::vec2(25, 25);
	// Add a Rune as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Rune", "Image/Rune.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);
	// Add a Key as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Key", "Image/Key.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Hard wood", "Image/Hard wood.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Stone Ore", "Image/Stone Ore.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Scrap Metal", "Image/Scrap Metal.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Pistol Bullets", "Image/Pistol Bullets.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Rifle Bullets", "Image/Rifle Bullets.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Rifle", "Image/Rifle.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Pistol", "Image/Pistol.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Medkit", "Image/Medkit.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Bandage", "Image/Bandage.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Fabric", "Image/Fabric.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Blueprint", "Image/Blueprint.tga", 1, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("empty", "Image/BlankBox.tga", 1, 0);
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
	// Store the old position
	vec2OldIndex = vec2Index;
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
	if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
	{
		// Calculate the new position to the left
		if (vec2Index.x >= 0)
		{
			vec2NumMicroSteps.x -= 1;
			if (vec2NumMicroSteps.x < 0)
			{
				vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
				vec2Index.x--;
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


		//Player is not idle
		idle = false;

		//Set Direction
		Direction = 0;

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
			vec2NumMicroSteps.x += 1;

			if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
			{
				vec2NumMicroSteps.x = 0;
				vec2Index.x++;
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
		//Player is not idle
		idle = false;

		//Set Direction
		Direction = 1;


		//CS: Play the "right" animation
		animatedSprites->PlayAnimation("right", -1, 1.0f);

		//CS: Init the color to white
		runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
	{
		// Calculate the new position up
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y += 1;
			if (vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				vec2NumMicroSteps.y = 0;
				vec2Index.y++;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(UP);
		// If the new position is not feasible, then revert to old position
		if (CheckPosition(UP) == false)
		{
			//vec2Index.y = vec2OldIndex.y;
			vec2NumMicroSteps.y = 0;
		}
		//Player is not idle
		idle = false;

		//Set Direction
		Direction = 2;


		//CS: Play the "up" animation
		animatedSprites->PlayAnimation("up", -1, 1.0f);

	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_S))
	{
		// Calculate the new position down
		if (vec2Index.y >= 0)
		{
			vec2NumMicroSteps.y -= 1;
			if (vec2NumMicroSteps.y < 0)
			{
				vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				vec2Index.y--;
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

		//Player is not idle
		idle = false;

		//Set Direction
		Direction = 3;


		//CS: Play the "down" animation
		animatedSprites->PlayAnimation("down", -1, 1.0f);
	}

	
	

	if (idle == true)
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

	


	//Interact with Map
	InteractWithMap();
	if (vec2Index.y - 1 > 0)
	{
		InteractWithMap(0, -1);
	}
	if (vec2Index.x + 1 < cSettings->NUM_TILES_XAXIS)
	{
		InteractWithMap(1, 0);
	}

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	{
		//Calculate Position of Entity on Screen
		glm::vec2 ScreenPos = glm::vec2(0, 0);

		//Check if Map View/Camera at Borders
		if (vec2Index.x < cSettings->VIEW_TILES_XAXIS * 0.5) // Left Side Border
		{
			ScreenPos.x = vec2Index.x + 1;
		}
		else if (vec2Index.x > (cSettings->NUM_TILES_XAXIS - (cSettings->VIEW_TILES_XAXIS * 0.5))) //Right Side Border
		{
			ScreenPos.x = cSettings->VIEW_TILES_XAXIS - (cSettings->NUM_TILES_XAXIS - vec2Index.x) + 1;
		}

		if (vec2Index.y > (cSettings->NUM_TILES_YAXIS - (cSettings->VIEW_TILES_YAXIS * 0.5))) //Top Side Border
		{
			ScreenPos.y = cSettings->VIEW_TILES_YAXIS - (cSettings->NUM_TILES_YAXIS - vec2Index.y) + 1;
		}
		else if (vec2Index.y < cSettings->VIEW_TILES_YAXIS * 0.5) //Bottom Side Border
		{
			ScreenPos.y = vec2Index.y + 1;
		}


		//If not at Border, Entity at Center of Screen displaced by x:1 y:1
		if (ScreenPos.x == 0)
		{
			ScreenPos.x = cSettings->VIEW_TILES_XAXIS * 0.5 + 1;
		}
		if (ScreenPos.y == 0)
		{
			ScreenPos.y = cSettings->VIEW_TILES_YAXIS * 0.5 + 1;
		}

		//Convert position to UV Coords
		vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, ScreenPos.x - 1, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
		vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, ScreenPos.y - 1, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
	}
	//std::cout << "x:" << vec2Index.x << " y:" << vec2Index.y << " Disx: " << cMap2D->GetDisplacement().x << " Disy: " << cMap2D->GetDisplacement().y  << " xUV:" << vec2UVCoordinate.x << " yUV:" << vec2UVCoordinate.y << std::endl;	
	//std::cout << "x:" << vec2Index.x << " y:" << vec2Index.y << " Microx: " << vec2NumMicroSteps.x << " Microy: " << vec2NumMicroSteps.y << std::endl;
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


unsigned int CPlayer2D_V2::GetDirection(void)
{
	return Direction;
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
	//Exit
	case 18:  
		cInventoryItem = cInventoryManager->GetItem("Rune");
		if (cInventoryItem->iItemCount == cInventoryItem->iItemMaxCount && (xdisplacement == 0 && ydisplacement == 0))
		{
			cInventoryItem = cInventoryManager->GetItem("Lives");
			cInventoryItem->iItemCount = cInventoryItem->GetMaxCount();
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

