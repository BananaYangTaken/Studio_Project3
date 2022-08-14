/**
 CScene2D
 @brief A class which manages the 2D game scene
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Scene2D.h"
#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

#include "System\filesystem.h"
#include "..\SoundController\SoundInfo.h"

//Enemies
#include "Enemy2D_V2.h"
#include "Enemy2D_Skeleton.h"
#include "Enemy2D_Human.h"
#include "Enemy2D_Creeper.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CScene2D::CScene2D(void) 
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	, Player1(NULL)
	, Player2(NULL)
	, cGUI_Scene2D(NULL)
	, cGameManager(NULL)
	, cSettings(NULL)
	, cSoundController(NULL)
	, cObjectList(NULL)
	, cEnemyList(NULL)
	, cProjectileManager(NULL)
{
}

/**
 @brief Destructor
 */
CScene2D::~CScene2D(void)
{
	if (cSoundController)
	{
		cSoundController = NULL;
	}
	if (cKeyboardController)
	{
		// We won't delete this since it was created elsewhereasd
		cKeyboardController = NULL;
	}
	if (cMap2D)
	{
		cMap2D->Destroy();
		cMap2D = NULL;
	}
	if (Player1)
	{
		delete Player1;
		Player1 = NULL;
	}
	if (Player2)
	{
		delete Player2;
		Player2 = NULL;
	}
	if (cGameManager)
	{
		cGameManager->Destroy();
		cGameManager = NULL;
	}
	if (cSettings)
	{
		cSettings = NULL;
	}

	for (unsigned int i = 0; i < cObjectList.size(); i++)
	{
		delete cObjectList[i];
		cObjectList[i] = NULL;
	}
	cObjectList.clear();

	for (unsigned int i = 0; i < cEnemyList.size(); i++)
	{
		delete cEnemyList[i];
		cEnemyList[i] = NULL;
	}
	cEnemyList.clear();

	cProjectileManager->Exit();
	CInventoryManager::GetInstance()->Exit();
	cGUI_Scene2D->Destroy();
}

/**
@brief Init Initialise this instance
*/ 
bool CScene2D::Init(void)
{
	// Include Shader Manager
	CShaderManager::GetInstance()->Use("Shader2D");
	//Load Scene2DColour into Shader Manager
	CShaderManager::GetInstance()->Use("Shader2D_Colour");
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	cKeyboardController = CKeyboardController::GetInstance();

	//Create and initialise the Map2D
	cMap2D = CMap2D::GetInstance();

	//Set shader to this class
	cMap2D->SetShader("Shader2D");

	//Initialse this instance
	if (cMap2D->Init(5, CSettings::GetInstance()->NUM_TILES_YAXIS, CSettings::GetInstance()->NUM_TILES_XAXIS) == false)
	{
		std::cout << "Failed to load CMap2D" << std::endl;
		return false;
	}
	//Loading Maps
	{
		//Load Map onto Array
		if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01.csv", 0) == false)
		{
			//Map loading failed, return false
			return false;
		}
		cMap2D->LoadMap("Maps/DM2213_Map_Level_01.csv", 0);

		if (cMap2D->LoadMap("Maps/DM2213_Map_Level_02.csv", 1) == false)
		{
			//Map loading failed, return false
			return false;
		}
		cMap2D->LoadMap("Maps/DM2213_Map_Level_02.csv", 1);

		//Load Map onto Array
		if (cMap2D->LoadMap("Maps/DM2213_Map_Level_03.csv", 2) == false)
		{
			//Map loading failed, return false
			return false;
		}
		cMap2D->LoadMap("Maps/DM2213_Map_Level_03.csv", 2);

		//Load Map onto Array
		if (cMap2D->LoadMap("Maps/DM2213_Map_Win.csv", 3) == false)
		{
			//Map loading failed, return false
			return false;
		}
		cMap2D->LoadMap("Maps/DM2213_Map_Win.csv", 3);

		//Load Map onto Array
		if (cMap2D->LoadMap("Maps/DM2213_Map_Lose.csv", 4) == false)
		{
			//Map loading failed, return false
			return false;
		}
		cMap2D->LoadMap("Maps/DM2213_Map_Lose.csv", 4);
	}
	//cMap2D->SetCurrentLevel(1);
	
	//Create and initialse players
	Player1 = new CPlayer2D_V2();
	Player2 = new CPlayer2D_V2();
	//Set Player Numbers
	Player1->SetPlayer(1);
	Player2->SetPlayer(2);
	//Pass shader to players
	Player1->SetShader("Shader2D_Colour");
	Player2->SetShader("Shader2D_Colour");
	//Initialise the players
	if (Player1->Init() == false)
	{
		std::cout << "Failed to load Player1" << std::endl;
		return false;
	}
	if (Player2->Init() == false)
	{
		std::cout << "Failed to load Player2" << std::endl;
		return false;
	}

	Player1->SetObjectList(&cObjectList);
	Player2->SetObjectList(&cObjectList);


	cProjectileManager = CProjectileManager::GetInstance();
	cProjectileManager->Init(Player1, Player2);
	cProjectileManager->SetEnemyVector(&cEnemyList);
	LoadObjects();
	LoadEnemies();
	//Create and initialse 2D GUI
	cGUI_Scene2D = CGUI_Scene2D::GetInstance();
	//Initialise the instance
	if (cGUI_Scene2D->Init() == false)
	{
		std::cout << "Failed to load CGUI_Scene2D" << std::endl;
		return false;
	}

	//Create and initialse Game Manager
	cGameManager = CGameManager::GetInstance();
	//Initialise the instance
	cGameManager->Init(); 
	cSoundController = CSoundController::GetInstance();
	cSettings->bDisableMousePointer = true;

	return true;
}

/**
@brief Update Update this instance
*/
bool CScene2D::Update(const double dElapsedTime)
{

	//Call Map2D's update method
	cMap2D->Update(dElapsedTime);

	//Call Players's update method
 	Player1->Update(dElapsedTime);
	Player2->Update(dElapsedTime);

	Player1->GetJumpCount() > Player2->GetJumpCount() ? Player2->SetJumpCount(Player1->GetJumpCount()) : Player1->SetJumpCount(Player2->GetJumpCount());
	Player1->GetTeleportPower() > Player2->GetTeleportPower() ? Player2->SetTeleportPower(Player1->GetTeleportPower()) : Player1->SetTeleportPower(Player2->GetTeleportPower());
	
	for (int i = 0; i < cObjectList.size(); i++)
	{
		cObjectList[i]->Update(dElapsedTime);
	}
	LoadObjects();
	for (int i = 0; i < cEnemyList.size(); i++)
	{
		cEnemyList[i]->Update(dElapsedTime);
	}
	LoadEnemies();
	vector<CEnemyBase*>::iterator it = cEnemyList.begin();
	while (it != cEnemyList.end())
	{
		// remove odd numbers
		if ((*it) && (*it)->GetHealth() <= 0)
		{
			it = cEnemyList.erase(it);
		}
		else {
			++it;
		}
	}
	if(Player2->Getattack() == true)
	{
		for (int i = 0; i < cEnemyList.size(); i++)
		{
			if (Player2->GetDirection() == 1) 
			{
				if(cEnemyList.at(i)->vec2Index.y == Player2->vec2Index.y 
				&& (cEnemyList.at(i)->vec2Index.x == Player2->vec2Index.x || cEnemyList.at(i)->vec2Index.x-1 == Player2->vec2Index.x) )
				{
					if (cEnemyList.at(i)->GetInvulnerabilityFrame() <= 0)
					{
						cEnemyList.at(i)->SetHealth(cEnemyList.at(i)->GetHealth() - 1);
						cEnemyList.at(i)->SetInvulnerabilityFrame(0.5);
						cEnemyList.at(i)->SetStun(true);
						CSoundController::GetInstance()->PlaySoundByID(4);
					}
				}
			}
			else if (Player2->GetDirection() == 0)
			{
				if (cEnemyList.at(i)->vec2Index.y == Player2->vec2Index.y
				&& (cEnemyList.at(i)->vec2Index.x == Player2->vec2Index.x || cEnemyList.at(i)->vec2Index.x + 1 == Player2->vec2Index.x))
				{
					if (cEnemyList.at(i)->GetInvulnerabilityFrame() <= 0)
					{
						cEnemyList.at(i)->SetHealth(cEnemyList.at(i)->GetHealth() - 1);
						cEnemyList.at(i)->SetInvulnerabilityFrame(0.5);
						cEnemyList.at(i)->SetStun(true);
						CSoundController::GetInstance()->PlaySoundByID(4);
					}
				}
			}
		}
	}
	cProjectileManager->Update(dElapsedTime);

	//Call cGUI's update method
	cGUI_Scene2D->SetPlayerHealth(Player1->GetHealth(), Player2->GetHealth());
	cGUI_Scene2D->Update(dElapsedTime);
	//Call Sound Controller's update method
	cSoundController->Update(dElapsedTime);

	// Get keyboard updates
	if (cKeyboardController->IsKeyDown(GLFW_KEY_F6))
	{
		//Save the current game to a save file
		//Make sure the file is open
		try
		{
			if (cMap2D->SaveMap("Maps/DM2213_Map_Level_01_SAVEGAMEtest.csv") == false)
			{
				throw runtime_error("Unable to save current game to file");
			}
		}
		catch (runtime_error e)
		{
			std::cout << "Runtime error: " << e.what();
			return false;
		}
	}

	if (cKeyboardController->IsKeyPressed('R'))
	{
		RestartLevel();
	}
	
	
	if (Player1->GetAtExit() && Player2->GetAtExit())
	{
		cGameManager->bLevelCompleted = true;
	}
	//check if the player can proceed to next level
	if (cGameManager->bLevelCompleted == true)
	{
		cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() + 1);
		cGameManager->bLevelCompleted = false;
		cGameManager->bLevelToReplay = true; 
		if (cMap2D->GetCurrentLevel() == 3)
		{
			cGameManager->bPlayerWon = true;
		}

	}
	//Check if player should restart the level
	if (cGameManager->bLevelToReplay == true)
	{
		RestartLevel();
		cGameManager->bLevelToReplay = false;
	}
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CScene2D::PreRender(void)
{
	// Reset the OpenGL rendering environment
	glLoadIdentity();

	// Clear the screen and buffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Enable 2D texture rendering
	glEnable(GL_TEXTURE_2D);
}

/**
 @brief Render Render this instance
 */
void CScene2D::Render(void)
{

	//Call Map2D's Pre Render()
	cMap2D->PreRender();
	//Call Map2D's Render()
	cMap2D->Render();
	//Call Map2D's Post Render()
	cMap2D->PostRender();

	//Call Player's Pre Render()
	Player1->PreRender();
	Player2->PreRender();
	//Call Player's Render()
	Player1->Render();
	Player2->Render();
	//Call Player's Post Render()
	Player1->PostRender();
	Player2->PostRender();
	for (int i = 0; i < cObjectList.size(); i++)
	{
		cObjectList[i]->PreRender();
		cObjectList[i]->Render();
		cObjectList[i]->PostRender();
	}

	for (unsigned int i = 0; i < cEnemyList.size(); i++)
	{
		//Call CEnemy2D's Pre Render()
		cEnemyList[i]->PreRender();
		//Call CEnemy2D's Render()
		cEnemyList[i]->Render();
		//Call CEnemy2D's Post Render()
		cEnemyList[i]->PostRender();
	}
	cProjectileManager->PreRender();
	cProjectileManager->Render();
	cProjectileManager->PostRender();

	//Call cGUI_Scene2D's Pre Render()
	cGUI_Scene2D->PreRender();
	//Call Player2D's Render()
	cGUI_Scene2D->Render();
	//Call Player2D's Post Render()
	cGUI_Scene2D->PostRender();
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CScene2D::PostRender(void)
{
}

void CScene2D::RestartLevel(void)
{
	if (cMap2D->GetCurrentLevel() == 0)
		cMap2D->LoadMap("Maps/DM2213_Map_Level_01.csv", 0);
	else if (cMap2D->GetCurrentLevel() == 1)
		cMap2D->LoadMap("Maps/DM2213_Map_Level_02.csv", 1);
	else if (cMap2D->GetCurrentLevel() == 2)
		cMap2D->LoadMap("Maps/DM2213_Map_Level_03.csv", 2);
	//else if (cMap2D->GetCurrentLevel() == 3)
		//cMap2D->LoadMap("Maps/DM2213_Map_Win.csv", 3);
	else if (cMap2D->GetCurrentLevel() == 4)
		cMap2D->LoadMap("Maps/DM2213_Map_Lose", 4);
	Player1->Reset();
	Player2->Reset();
	for (unsigned int i = 0; i < cObjectList.size(); i++)
	{
		delete cObjectList[i];
		cObjectList[i] = NULL;
	}
	cObjectList.clear();

	for (unsigned int i = 0; i < cEnemyList.size(); i++)
	{
		delete cEnemyList[i];
		cEnemyList[i] = NULL;
	}
	cEnemyList.clear();
	//Load Objects
	LoadObjects();
	//Load Enemy
	LoadEnemies();

	cProjectileManager->Reset();
}

void CScene2D::LoadObjects(void)
{
	Grid*** arrMapInfo;
	arrMapInfo = cMap2D->GetarrMapInfo();
	unsigned int uiCurlLevel = cMap2D->GetCurrentLevel();
	// Read the rows and columns of CSV data into arrMapInfo
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		// Load a particular CSV value into the arrMapInfo
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
		{
			int Value = arrMapInfo[uiCurlLevel][uiRow][uiCol].value;
			if (Value == 24 || Value == 25 || Value == 26 || Value == 27 || Value == 28)
			{
				bool Objectnew = true;
				for (int i = 0; i < cObjectList.size(); i++)
				{
					if (cObjectList.at(i)->vec2Index.x == uiCol && cObjectList.at(i)->vec2Index.y == cSettings->NUM_TILES_YAXIS - uiRow - 1)
					{
						Objectnew = false;
						break;
					}
				}
				if (Objectnew == true)
				{
					CObject2D* cObject2D;
					cObject2D = new CObject2D(Value);
					cObject2D->SetShader("Shader2D_Colour");
					cObject2D->Init(uiCol, cSettings->NUM_TILES_YAXIS-uiRow-1 );
					cObjectList.push_back(cObject2D);
				}
			}
		}
	}
}

void CScene2D::LoadEnemies(void)
{
	Grid*** arrMapInfo;
	arrMapInfo = cMap2D->GetarrMapInfo();
	unsigned int uiCurlLevel = cMap2D->GetCurrentLevel();
	// Read the rows and columns of CSV data into arrMapInfo
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		// Load a particular CSV value into the arrMapInfo
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
		{
			int Value = arrMapInfo[uiCurlLevel][uiRow][uiCol].value;
			if (Value >= 300)
			{
				bool Enemynew = true;
				for (int i = 0; i < cEnemyList.size(); i++)
				{
					if (cEnemyList.at(i)->vec2Index.x == uiCol && cEnemyList.at(i)->vec2Index.y == cSettings->NUM_TILES_YAXIS - uiRow - 1)
					{
						Enemynew = false;
						break;
					}
				}
				if (Enemynew == true)
				{
					CEnemyBase* cEnemy2D;
					if (Value == 300)
						cEnemy2D = new CEnemy2D_V2();
					else if (Value == 301)
						cEnemy2D = new CEnemy2D_Skeleton();
					else if (Value == 302)
						cEnemy2D = new CEnemy2D_Human();
					else if (Value == 303)
						cEnemy2D = new CEnemy2D_Creeper();
					// Pass the shader to instance
					cEnemy2D->SetShader("Shader2D_Colour");
					if (cEnemy2D->Init(uiCol, cSettings->NUM_TILES_YAXIS - uiRow - 1, cProjectileManager->GetProjectileList()) == true)
					{
						cEnemy2D->SetPlayer2D(Player1, Player2);
						cEnemyList.push_back(cEnemy2D);
					}
				}
			}
		}
	}
}

void CScene2D::Destroy(void)
{
	if (cSoundController)
	{
		cSoundController = NULL;
	}
	if (cKeyboardController)
	{
		// We won't delete this since it was created elsewhere
		cKeyboardController = NULL;
	}
	if (cMap2D)
	{
		cMap2D->Destroy();
		cMap2D = NULL;
	}
	if (Player1)
	{
		delete Player1;
		Player1 = NULL;
	}
	if (Player2)
	{
		delete Player2;
		Player2 = NULL;
	}
	if (cGameManager)
	{
		cGameManager->Destroy();
		cGameManager = NULL;
	}
	if (cSettings)
	{
		cSettings = NULL;
	}

	for (unsigned int i = 0; i < cObjectList.size(); i++)
	{
		delete cObjectList[i];
		cObjectList[i] = NULL;
	}
	cObjectList.clear();

	for (unsigned int i = 0; i < cEnemyList.size(); i++)
	{
		delete cEnemyList[i];
		cEnemyList[i] = NULL;
	}
	cEnemyList.clear();
	cGUI_Scene2D->Destroy();
}
