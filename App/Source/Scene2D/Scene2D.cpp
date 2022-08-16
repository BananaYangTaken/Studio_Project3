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
#include "Physics2D.h"
#include "..\Application.h"

//Enemies
#include "Enemy2D_Zombie.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CScene2D::CScene2D(void) 
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	, Player(NULL)
	, cGUI_Scene2D(NULL)
	, cGameManager(NULL)
	, cSettings(NULL)
	, cSoundController(NULL)
	, cObjectList(NULL)
	, cEnemyList(NULL)
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
	if (Player)
	{
		Player->Destroy();
		Player = NULL;
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

	cObjectList.clear();
	cEnemyList.clear();

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
	cPhysics2D.Init();
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
		if (cMap2D->LoadMap("Maps/Map.csv", 0) == false)
		{
			//Map loading failed, return false
			return false;
		}
		cMap2D->LoadMap("Maps/Map.csv", 0);
	}
	//cMap2D->SetCurrentLevel(1);
	
	//Create and initialse players
	Player = CPlayer2D_V2::GetInstance();
	Player->SetShader("Shader2D_Colour");
	//Initialise the players
	if (Player->Init() == false)
	{
		std::cout << "Failed to load Player" << std::endl;
		return false;
	}

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
 	Player->Update(dElapsedTime);
	
	
	for (int i = 0; i < cObjectList.size(); i++)
	{
		cObjectList[i]->SetPlayervec2Index(Player->vec2Index);
		cObjectList[i]->Update(dElapsedTime);
		cObjectList[i]->SetruntimeColour(glm::vec4(1, 1, 1, 1));
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
		if ((*it) && (*it)->GetHealth() <= 0 && (*it)->GetDeathState() == 1)
		{
			it = cEnemyList.erase(it);
		}
		else {
			++it;
		}
	}
	
	//Call cGUI's update method
	cGUI_Scene2D->Update(dElapsedTime);
	//Call Sound Controller's update method
	cSoundController->Update(dElapsedTime);

	cMap2D->vec2Index = Player->vec2Index;

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

	
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_TAB))
	{
		if (cGUI_Scene2D->checkinginventory == false)
		{
			cGUI_Scene2D->checkinginventory = true;
			cGUI_Scene2D->crafting = false;
		}
		else
		{
			cGUI_Scene2D->checkinginventory = false;
			cGUI_Scene2D->crafting = false;
		}
	}
	if (cKeyboardController->IsKeyPressed('U'))
	{
		if (cGUI_Scene2D->crafting == false)
		{
			cGUI_Scene2D->crafting = true;
			cGUI_Scene2D->checkinginventory = false;
		}
		else
		{
			cGUI_Scene2D->crafting = false;
			cGUI_Scene2D->checkinginventory = false;
		}
	}
	if (cKeyboardController->IsKeyPressed('X'))
	{
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
	
	//Mouse Position
	double x, y;
	Application::GetCursorPos(&x, &y);
	float mousexpos = ((x / cSettings->iWindowWidth) * cSettings->VIEW_TILES_XAXIS);
	float mouseypos = (abs((y - cSettings->iWindowHeight) / cSettings->iWindowHeight) * cSettings->VIEW_TILES_YAXIS);

	//Convert Mouse Position to vec2UVCoords
	glm::vec2 MouseUVCoords;
	MouseUVCoords.x = (mousexpos - cSettings->VIEW_TILES_XAXIS * 0.5) / (cSettings->VIEW_TILES_XAXIS * 0.5);
	MouseUVCoords.y = (mouseypos - cSettings->VIEW_TILES_YAXIS * 0.5) / (cSettings->VIEW_TILES_YAXIS * 0.5);
	
	//Hovered Object green
	for (int i = 0; i < cObjectList.size(); ++i)
	{
		//Check if object within reach of player
		if (cPhysics2D.CalculateDistance(cObjectList[i]->vec2UVCoordinate, MouseUVCoords) <= 1/cSettings->VIEW_TILES_XAXIS)
		{
			cObjectList[i]->SetruntimeColour(glm::vec4(0,1,0,1));
		}
	}
	std::cout << cPhysics2D.CalculateDistance(cObjectList[0]->vec2UVCoordinate, MouseUVCoords) << std::endl;
	//cObjectList[i]->SetruntimeColour(glm::vec4(0, 1, 0, 1));
	if (cKeyboardController->IsKeyPressed('F'))
	{
		for (int i = 0; i < cObjectList.size(); ++i)
		{
			//Check if object within reach of player
			if (cPhysics2D.CalculateDistance(cObjectList[i]->vec2Index, Player->vec2Index) <= 1)
			{
				//cObjectList[i]->Interact();
			}
		}
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
	Player->PreRender();
	//Call Player's Render()
	Player->Render();
	//Call Player's Post Render()
	Player->PostRender();
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
					cObject2D->SetPlayervec2Index(Player->vec2Index);
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
					if (Value == 302)
						cEnemy2D = new CEnemy2D_Zombie();
					//else if (Value == 303)
					//	cEnemy2D = new CEnemy2D_Creeper();
					// Pass the shader to instance
					cEnemy2D->SetShader("Shader2D_Colour");
					if (cEnemy2D->Init(uiCol, cSettings->NUM_TILES_YAXIS - uiRow - 1))
					{
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
	if (Player)
	{
		Player->Destroy();
		Player = NULL;
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
	if (cGUI_Scene2D)
	{
		cGUI_Scene2D->Destroy();
		cGUI_Scene2D = NULL;
	}
	cObjectList.clear();
	cEnemyList.clear();
}
