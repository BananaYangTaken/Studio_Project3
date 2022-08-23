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
#include "Enemy2D_Nurse.h"
#include "Enemy2D_Police.h"
#include "Enemy2D_Mutant.h"

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
	, cTurretList(NULL)
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
	cTurretList.clear();

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
	cSoundController = CSoundController::GetInstance();
	//Create and initialise the Map2D
	cMap2D = CMap2D::GetInstance();

	//Set shader to this class
	cMap2D->SetShader("Shader2D");

	WindowUpgradeLvl = 0;
	TurretUpgradeLvl = 0;
	BarbwireUpgrade = 0;

	daylightTimer = 0;
	hours = 18;
	mins = 0;
	days = 1;
	spawnrate = 1;
	isNight = false;
	SolarEclipse = false;
	BloodMoon = false;
	numSpawned = 0;

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
	Player->SetObjectList(&cObjectList);
	Player->SetEnemyList(&cEnemyList);

	LoadObjects();
	LoadEnemies();
	LoadTurret();

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

	cProjectileManager = cProjectileManager->GetInstance();
	cProjectileManager->Init();
	cProjectileManager->SetEnemyVector(&cEnemyList);
	return true;
}

void CScene2D::UpdateDaylightCycle(const double dElapsedTime)
{
	daylightTimer += dElapsedTime;
	if (daylightTimer >= 0.1)
	{
		mins += 1;
		daylightTimer = 0;
	}
	if (mins >= 60)
	{
		mins = 0;
		hours++;
	}
	if (hours >= 24)
	{
		hours = 0;
		days++;
	}
}

void CScene2D::SpawnEnemies()
{
	// 302 Regular Zombie
	// 303 Nurse Zombie
	// 304 Police Zombie
	// 305 Mutant Zombie

	int EnemyRand = Math::RandIntMinMax(1, 20);
	int randx = 0;
	int randy = 0;


	
	//mutant zombie
	if (EnemyRand == 20)
	{
		// EVERYWHERE
		randx = Math::RandIntMinMax(25, 90);
		randy = Math::RandIntMinMax(10, 90);
		while (cMap2D->GetMapInfo(randy, randx) != 0)
		{
			randx = Math::RandIntMinMax(25, 90);
			randy = Math::RandIntMinMax(10, 90);
		}
		cMap2D->SetMapInfo(randy, randx, 305);
	}
	//police zombie
	else if (EnemyRand >= 16 && EnemyRand <= 19)
	{
		int MilOrStn = Math::RandIntMinMax(1, 3);

		if (MilOrStn == 1)
		{
			// POLICE STATION
			randx = Math::RandIntMinMax(5, 24);
			randy = Math::RandIntMinMax(85, 95);
			while (cMap2D->GetMapInfo(randy, randx) != 0)
			{
				randx = Math::RandIntMinMax(5, 24);
				randy = Math::RandIntMinMax(85, 95);
			}
		}
		else if (MilOrStn == 2)
		{
			// MILITARY BASE
			randx = Math::RandIntMinMax(45, 98);
			randy = Math::RandIntMinMax(3, 10);
			while (cMap2D->GetMapInfo(randy, randx) != 0)
			{
				randx = Math::RandIntMinMax(45, 98);
				randy = Math::RandIntMinMax(3, 10);
			}
		}
		else if (MilOrStn == 3)
		{
			// GENERAL AREA
			randx = Math::RandIntMinMax(37, 67);
			randy = Math::RandIntMinMax(48, 74);
			while (cMap2D->GetMapInfo(randy, randx) != 0)
			{
				randx = Math::RandIntMinMax(37, 67);
				randy = Math::RandIntMinMax(48, 74);
			}
		}
		cMap2D->SetMapInfo(randy, randx, 304);
	}
	//nurse zombie
	else if (EnemyRand >= 11 && EnemyRand <= 15)
	{
		int SpawnWhere = Math::RandIntMinMax(1, 2);
		// HOSPITAL
		if (SpawnWhere == 1)
		{
			randx = Math::RandIntMinMax(80, 98);
			randy = Math::RandIntMinMax(60, 98);
			while (cMap2D->GetMapInfo(randy, randx) != 0)
			{
				randx = Math::RandIntMinMax(80, 98);
				randy = Math::RandIntMinMax(60, 98);
			}
		}
		else if (SpawnWhere == 2)
		{
			// GENERAL AREA
			randx = Math::RandIntMinMax(37, 67);
			randy = Math::RandIntMinMax(48, 74);
			while (cMap2D->GetMapInfo(randy, randx) != 0)
			{
				randx = Math::RandIntMinMax(37, 67);
				randy = Math::RandIntMinMax(48, 74);
			}
		}
		cMap2D->SetMapInfo(randy, randx, 303);
	}
	//regular zombie
	else if (EnemyRand >= 1 && EnemyRand <= 10)
	{
		// EVERYWHERE
		randx = Math::RandIntMinMax(25, 90);
		randy = Math::RandIntMinMax(10, 90);
		while (cMap2D->GetMapInfo(randy, randx) != 0)
		{
			randx = Math::RandIntMinMax(25, 90);
			randy = Math::RandIntMinMax(10, 90);
		}
		cMap2D->SetMapInfo(randy, randx, 302);
	}
}

void CScene2D::BloodMoonOrSolarEclipse()
{
	int EventBaseChance = Math::RandIntMinMax(1, 100);
	int TotalChance = 1 + days;
	cout << EventBaseChance << endl;
	if (EventBaseChance <= TotalChance)
	{
		if (hours == 18 && mins == 1)
		{
			BloodMoon = true;
			cout << "BLOOD MOON" << endl;
		}
		else if (hours == 6 && mins == 1)
		{
			SolarEclipse = true;
			cout << "SOLAR ECLIPSE" << endl;
		}
	}
}

/**
@brief Update Update this instance
*/
bool CScene2D::Update(const double dElapsedTime)
{
	cout << hours << endl;
	string temp = "Days : ";
	cGUI_Scene2D->days = temp + std::to_string(days);
	if (cKeyboardController->IsKeyPressed('='))
	{
		hours++;
	}
	BloodMoonOrSolarEclipse();
	
	if (BloodMoon == true || SolarEclipse == true)
	{
		spawnrate = 2 + (0.2 * days);
	}
	else
	{
		spawnrate = 1 + (0.1 * days);
	}
	int spawncount = 20 * spawnrate;
	UpdateDaylightCycle(dElapsedTime);
	if (hours >= 22 && BloodMoon == false && numSpawned == 0)
	{
		for (int i = 0; i < spawncount; i++)
		{
			SpawnEnemies();
		}
		numSpawned = 1;
	}
	else if (hours >= 20 && BloodMoon == true && numSpawned == 0)
	{
		for (int i = 0; i < spawncount; i++)
		{
			SpawnEnemies();
		}
		numSpawned = 1;
	}
	else if (SolarEclipse == true && hours > 6 && hours < 18 && numSpawned == 0)
	{
		for (int i = 0; i < spawncount; i++)
		{
			SpawnEnemies();
		}
		numSpawned = 1;
	}
	else if (hours >= 5 && hours < 18 && SolarEclipse == false)
	{
		numSpawned = 0;
		if (BloodMoon == true)
		{
			BloodMoon = false;
		}
	}
	if (hours >= 18 && SolarEclipse == true)
	{
		numSpawned = 0;
		SolarEclipse = false;
	}
	else if (SolarEclipse == true && hours > 5 && hours < 18)
	{
		if (BloodMoon == true)
		{
			BloodMoon = false;
		}
	}




	if (hours >= 19 && cGUI_Scene2D->darkenmap == false && BloodMoon == false)
	{
		cGUI_Scene2D->darkenmap = true;
		cGUI_Scene2D->DayNightIcon = "Sunrise";
	}
	else if (hours >= 19 && cGUI_Scene2D->darkenmap == false && BloodMoon == true)
	{
		cGUI_Scene2D->darkenmap = true;
		cGUI_Scene2D->redness = 0.5;
		cGUI_Scene2D->DayNightIcon = "Sunrise";

	}
	else if(hours >= 21 && cGUI_Scene2D->darkenmap == true && calledonce == false)
		cGUI_Scene2D->DayNightIcon = "Night";




	if (hours >= 22 && cGUI_Scene2D->darkenmap == true && calledonce == false)
	{
		cSoundController->PlaySoundByID(39);
		cSoundController->PlaySoundByID(39);
		cout << "HORDE SPAWNED!";
		calledonce = true;
	}
	if (hours >= 7 && hours <= 12 && cGUI_Scene2D->darkenmap == false)
	{
		if (spawnloot == false)
		{
			cGUI_Scene2D->clearmap();
			cGUI_Scene2D->SpawnRandomHomeDepot();
			cGUI_Scene2D->SpawnRandomHospital();
			cGUI_Scene2D->SpawnRandomMilBase();
			cGUI_Scene2D->SpawnRandomPoliceStation();
			cGUI_Scene2D->SpawnRandomYellowRoom();
			spawnloot = true;
		}
		cGUI_Scene2D->DayNightIcon = "Day";
		calledonce = false;
	}
	else if (hours >= 6 && hours <= 12 && cGUI_Scene2D->darkenmap == true && SolarEclipse == false)
	{
		cGUI_Scene2D->darkenmap = false;
		cGUI_Scene2D->DayNightIcon = "Sunrise";
	}



	if (hours >= 6 && hours <= 12 && cGUI_Scene2D->darkenmap == false && SolarEclipse == true)
	{
		spawnloot = false;
		cGUI_Scene2D->darkenmap = false;
		cGUI_Scene2D->redness = 0.5;
		cGUI_Scene2D->DayNightIcon = "Sunrise";
	}
	
	//Call Map2D's update method
	cMap2D->Update(dElapsedTime);

	//Call Players's update method
	Player->Update(dElapsedTime);
	
	LoadObjects();
	for (int i = 0; i < cObjectList.size(); i++)
	{
		cObjectList[i]->SetPlayervec2(Player->vec2Index, Player->vec2NumMicroSteps);
		cObjectList[i]->Update(dElapsedTime);
		cObjectList[i]->SetruntimeColour(glm::vec4(1, 1, 1, 1));
	}
	LoadEnemies();
	vector<CEntity2D*>::iterator it = cEnemyList.begin();
	while (it != cEnemyList.end())
	{
		CEnemyBase* Temp = static_cast<CEnemyBase*>(*it);
		// remove odd numbers
		if (/*(*it) && Temp->GetHealth() <= 0 && */Temp->bIsActive == false)
		{
			it = cEnemyList.erase(it);
		}
		else {
			(*it)->Update(dElapsedTime);
			++it; 
		}
	}
	
	
	for (int i = 0; i < cTurretList.size(); i++)
	{
		cTurretList[i]->Update(dElapsedTime);
		cTurretList[i]->SetTurretLevel(TurretUpgradeLvl);
	}
	LoadTurret();
	cProjectileManager->Update(dElapsedTime);


	//Call cGUI's update method
	cGUI_Scene2D->Update(dElapsedTime);
	//Call Sound Controller's update method
 	cSoundController->Update(dElapsedTime);

	cMap2D->vec2Index = Player->vec2Index;
	cMap2D->vec2NumMicroSteps = Player->vec2NumMicroSteps;

	// Get keyboard updates
	if (cKeyboardController->IsKeyDown(GLFW_KEY_F10))
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
			cGUI_Scene2D->inventoryloaded = false;
			cGUI_Scene2D->checkinginventory = true;
			cGUI_Scene2D->crafting = false;
		}
		else
		{
			cGUI_Scene2D->checkinginventory = false;
			cGUI_Scene2D->crafting = false;
			cGUI_Scene2D->descactive = false;
		}
	}
	if (cKeyboardController->IsKeyPressed('U'))
	{
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
		//Check if mouse hovering over object
		if (cPhysics2D.CalculateDistance(cObjectList[i]->vec2UVCoordinate, MouseUVCoords) <= 1.f/cSettings->VIEW_TILES_XAXIS)
		{
			cObjectList[i]->SetruntimeColour(glm::vec4(0,1,0,1));
		}
	}
	if (cKeyboardController->IsKeyPressed('F'))
	{
		for (int i = 0; i < cObjectList.size(); ++i)
		{
			//Check if mouse hovering over object and whether player within radius to interact
			if (cPhysics2D.CalculateDistance(cObjectList[i]->vec2UVCoordinate, MouseUVCoords) <= 1.f / cSettings->VIEW_TILES_XAXIS
				&& cPhysics2D.CalculateDistance(Player->vec2Index, cObjectList[i]->vec2Index) <= 1)
			{
				cObjectList[i]->Interact();
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

	for (unsigned int i = 0; i < cTurretList.size(); i++)
	{
		//Call CTurret2D's Pre Render()
		cTurretList[i]->PreRender();
		//Call CTurret2D's Render()
		cTurretList[i]->Render();
		//Call CTurret2D's Post Render()
		cTurretList[i]->PostRender();
	}

	//Call cGUI_Scene2D's Pre Render()
	cGUI_Scene2D->PreRender();
	//Call Player2D's Render()
	cGUI_Scene2D->Render();
	//Call Player2D's Post Render()
	cGUI_Scene2D->PostRender();

	//Call cProjectileManager's Pre Render()
	cProjectileManager->PreRender();
	//Call cProjectileManager's Render()
	cProjectileManager->Render();
	//Call cProjectileManager's Post Render()
	cProjectileManager->PostRender();
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
			if (Value == 25 || Value == 26 || Value == 27 || Value == 28)
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
					cObject2D->SetPlayervec2(Player->vec2Index, Player->vec2NumMicroSteps);
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
			if (Value >= 300 && Value < 400)
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
					if (Value == 303)
						cEnemy2D = new CEnemy2D_Nurse();
					if (Value == 304)
						cEnemy2D = new CEnemy2D_Police();
					if (Value == 305)
						cEnemy2D = new CEnemy2D_Mutant();
					// Pass the shader to instance
					cEnemy2D->SetShader("Shader2D_Colour");
					cEnemy2D->SetPlayer(dynamic_cast<CEntity2D*>(Player));
					if (cEnemy2D->Init(uiCol, cSettings->NUM_TILES_YAXIS - uiRow - 1))
					{
						cEnemyList.push_back(cEnemy2D);
					}
				}
			}
		}
	}
}

void CScene2D::LoadTurret(void)
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
			if (Value >= 400)
			{
				bool Turretnew = true;
				for (int i = 0; i < cTurretList.size(); i++)
				{
					if (cTurretList.at(i)->vec2Index.x == uiCol && cTurretList.at(i)->vec2Index.y == cSettings->NUM_TILES_YAXIS - uiRow - 1)
					{
						Turretnew = false;
						break;
					}
				}
				if (Turretnew == true)
				{
					CTurretBase* cTurret2D;
					if (Value == 401)
						cTurret2D = new CTurretBase();
					cTurret2D->SetShader("Shader2D_Colour");
					if (cTurret2D->Init(uiCol, cSettings->NUM_TILES_YAXIS - uiRow - 1))
					{
						cTurret2D->SetEnemyList(&cEnemyList);
						cTurretList.push_back(cTurret2D);
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
	cTurretList.clear();
	if (cProjectileManager)
	{
		cProjectileManager->Destroy();
		cProjectileManager = NULL;
	}
}

int CScene2D::GetWindowUpgradeLvl()
{
	return WindowUpgradeLvl;
}

int CScene2D::GetTurretUpgradeLvl()
{
	return TurretUpgradeLvl;
}

int CScene2D::GetBarbwireUpgradeLvl()
{
	return BarbwireUpgrade;
}

void CScene2D::SetWindowUpgradeLvl(int newLV)
{
	WindowUpgradeLvl = newLV;
}

void CScene2D::SetTurretUpgradeLvl(int newLV)
{
	TurretUpgradeLvl = newLV;
}

void CScene2D::SetBarbwireUpgradeLvl(int newLV)
{
	BarbwireUpgrade = newLV;
}

