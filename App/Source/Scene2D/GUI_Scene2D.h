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
	bool selectinventory = true;
	const int inventory_size = 9;
	std::string inventory_item_name_list[9] = {"Scrap Metal", "Hard wood", "Stone Ore", "Pistol Bullets", "Rifle Bullets", "Rifle", "Pistol", "Medkit", "Bandage"};
	int inventory_item_quantity[9] = {155,150,150,100,50,1,1,90,200};
	int inventory_item_max_quantity[9] = {200,200,200,200,200,200,200,200,200};

	int Crafting_list_size = 5;
	std::string Crafting_item_name_list[5] = { "Rifle Bullets", "Pistol Bullets", "Scrap Metal","Bandage","Medkit" };
	int Crafting_item_Quantity[5] = {25,50,30,5,1};
	int reckey = 999, deskey;
	std::string description = "";

	int Chest_size = 8;
	std::string chest_item_name_list[8] = { "empty2", "empty3", "empty4", "empty5", "empty6", "empty7", "empty8", "empty9"};
	int chest_item_quantity[8] = { 0,0,0,0,0,0,0,0};
	int chest_item_max_quantity[8] = { 999,999,999,999,999,999,999,999};
	
	int lootcrateSize = 8;
	std::string crate_item_name_list[8] = { "empty2", "empty3", "empty4", "empty5", "empty6", "empty7", "empty8", "empty9" };
	int crate_item_quantity[8] = { 0,0,0,0,0,0,0,0 };
	int crate_item_max_quantity[8] = { 15,15,15,15,15,15,15,15};


	int chestactive = false;
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
	// Update
	void Update(const double dElapsedTime);
	void setInventoryItem(int arrayVal, std::string item, int quantity, int maxQuantity);
	int IncreaseInventoryItemCount(std::string arrayindex, int incrementValue);
	void DecreaseInventoryItemCount(std::string arrayindex, int decrementvalue);
	bool CheckCrafting(int recipeIngredientCount, std::string Ingredients[4], int IngredientRequiredCount[4], std::string ResultantCraft, int CraftedQuantity);
	void InventoryItemSetZero(std::string arrayindex);
	void ChestItemSetZero(std::string arrayindex);
	void checkforzero(void);
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

	// PostRender
	void PostRender(void);
	void Destroy(void);

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
	ButtonData Add1Button;
	ButtonData Add10Button;
	ButtonData AddAllButton;
	ButtonData newInventorybutton;
	ButtonData newDropbutton;
	ButtonData newChestbutton;
	ButtonData newDescriptionButton;
	ButtonData newTransferButton;
	// These variables are for IMGUI demo only
	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;


};
