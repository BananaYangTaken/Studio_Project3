/**
 CGUI_Scene2D
 @brief A class which manages the GUI for Scene2D
 By: Toh Da Jun
 Date: May 2021
 */
#pragma once

// Include SingletonTemplate
#include "DesignPatterns/SingletonTemplate.h"
#include "Map2D.h"
#include "System\ImageLoader.h"
// Include CEntity2D
#include "Primitives/Entity2D.h"
// FPS Counter
#include "TimeControl\FPSCounter.h"

// Include CInventoryManager
#include "InventoryManager.h"
#include "..\SoundController\SoundController.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>
#include "Inputs\KeyboardController.h"
// Include IMGUI
// Important: GLEW and GLFW must be included before IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif

#include "GameControl/Settings.h"

#include <string>
using namespace std;

class CGUI_Scene2D : public CSingletonTemplate<CGUI_Scene2D>, public CEntity2D
{
	friend CSingletonTemplate<CGUI_Scene2D>;
public:
	// Init
	bool Init(void);
	bool darkenmap = false;
	void clearmap();
	float redness = 0;
	float blueness = 0;
	std::string DayNightIcon = "Morning";
	bool healing = false;
	float healamt = 0;
	float healtime = 0;
	bool textalready = false;
	bool flashlightequipped = false;
	bool muzzleequipped = false;
	bool magequipped = false;
	std::string timestring;
	float greenness = 0;
	std::string rifledescription;
	float transparency = 0;
	bool selectinventory = true;
	void spawnloot(float vecX, float vecY);
	const int inventory_size = 9;
	std::string inventory_item_name_list[9] = {"Rifle", "Flashlight", "Extended Magazine", "Pistol Bullets",  "Rifle Bullets", "MuzzleDevice", "Pistol", "Medkit", "Blueprint"};
	int inventory_item_quantity[9] = {1,999,999,999,999,5,5,90,200};
	int inventory_item_max_quantity[9] = {999,999,999,999,999,999,999,999,999};
	std::string searchtxt;
	bool issearched = false;
	float transparencyLimit;
	float searchtimer = 0;
	int chestactive = false;
	std::string actiontext;
	std::string actiontext2;
	int Crafting_list_size = 5;
	std::string Crafting_item_name_list[5] = { "Rifle Bullets", "Pistol Bullets", "Scrap Metal","Bandage","Medkit" };
	int Crafting_item_Quantity[5] = {25,50,30,5,1};
	int reckey = 999, deskey;
	std::string description = "";

	int Chest_size = 8;
	std::string chest_item_name_list[8] = { "empty2", "empty3", "empty4", "empty5", "empty6", "empty7", "empty8", "empty9"};
	int chest_item_quantity[8] = { 0,0,0,0,0,0,0,0};
	int chest_item_max_quantity[8] = { 999,999,999,999,999,999,999,999};
	
	void SpawnRandomPoliceStation();
	void SpawnRandomHospital();
	void SpawnRandomHomeDepot();
	void SpawnRandomMilBase();
	void SpawnRandomYellowRoom();
	bool CheckIfFull(std::string as);
	int lootcrateSize = 8;
	std::string crate_item_name_list[8] = { "empty2", "empty3", "empty4", "empty5", "empty6", "empty7", "empty8", "empty9" };
	int crate_item_quantity[8] = { 0,0,0,0,0,0,0,0 };
	int crate_item_max_quantity[8] = { 15,15,15,15,15,15,15,15};

	int chestkey = 1;
	float buttonWidth = 64;
	float buttonHeight = 64;
	int hotbarlevel = 0, OGclicked;
	bool inventoryloaded = false;
	bool checkinginventory = false;
	bool crafting = false;
	bool recipeactive = false;

	int hotbarselection = 1;
	bool descactive = false;
	string days;

	// Update
	void Update(const double dElapsedTime);
	void setInventoryItem(int arrayVal, std::string item, int quantity, int maxQuantity);
	int IncreaseInventoryItemCount(std::string arrayindex, int incrementValue);
	void DecreaseInventoryItemCount(std::string arrayindex, int decrementvalue);
	int checkifItemExists(std::string arrayindex);
	bool CheckCrafting(int recipeIngredientCount, std::string Ingredients[4], int IngredientRequiredCount[4], std::string ResultantCraft, int CraftedQuantity);
	void InventoryItemSetZero(std::string arrayindex);
	void ChestItemSetZero(std::string arrayindex);
	void checkforzero(void);
	std::string GetCurrentHotbarItem(void);
	void dropitem(int key);
	void OpenCrate(std::string size);
	bool looting = false;
	// PreRender
	void PreRender(void);
	void SwapItems(int itemindex, int swapindex);
	bool swapactive = false;
	int TransferToChest(std::string itemName, int quantity);
	int TransferTohand(std::string itemName, int quantity);
	void decreaseChestQuantity(std::string itemname, int quantity);
	// Render
	void Render(void);
	void PreloadInventoryTextures(void);
	bool hoveroveritem = false;
	float flooritemquantity = 0;
	std::string flooritemname;
	// PostRender
	void PostRender(void);
	void Destroy(void);
	ImVec2 PStopleft, PStopright, PSbottomleft, PSbottomright;
protected:
	struct ButtonData
	{
		std::string fileName;
		unsigned textureID;
	}; 
	

	// Constructor
	CGUI_Scene2D(void);

	// Destructor
	virtual ~CGUI_Scene2D(void);

	// FPS Control
	CFPSCounter* cFPSCounter;

	// Flags for IMGUI
	ImGuiWindowFlags window_flags;
	// For progress bar demo only
	float m_fProgressBar;
	CKeyboardController* cKeyboardController;
	CImageLoader* il;
	CMap2D* cMap2D;
	// The handler containing the instance of CInventoryManager
	CInventoryManager* cInventoryManager;
	// The handler containing the instance of CInventoryItem
	CInventoryItem* cInventoryItem;
	CSoundController* cSoundController;
	ButtonData Add1Button;
	ButtonData Add10Button;
	ButtonData AddAllButton;
	ButtonData DayNight;
	ButtonData newInventorybutton;
	ButtonData newDropbutton;
	ButtonData newChestbutton;
	ButtonData newLootButton;
	ButtonData newDescriptionButton;
	ButtonData newTransferButton;
	// These variables are for IMGUI demo only
	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;


};
