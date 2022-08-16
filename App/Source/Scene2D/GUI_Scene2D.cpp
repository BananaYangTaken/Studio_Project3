/**
 CGUI_Scene2D
 @brief A class which manages the GUI for Scene2D
 By: Toh Da Jun
 Date: May 2021
 */
#include "GUI_Scene2D.h"

#include <iostream>
 // Include ImageLoader
#include "System\ImageLoader.h"
using namespace std;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::CGUI_Scene2D(void)
	: window_flags(0)
	, m_fProgressBar(0.0f)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::~CGUI_Scene2D(void)
{
	if (cInventoryManager)
	{
		cInventoryManager->Destroy();
		cInventoryManager = NULL;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// We won't delete this since it was created elsewhere
	cSettings = NULL;
}

/**
  @brief Initialise this instance
  */
void CGUI_Scene2D::setInventoryItem(int arrayVal, std::string item, int quantity, int maxQuantity)
{
	inventory_item_name_list[arrayVal] = item;
	inventory_item_quantity[arrayVal] = quantity;
	inventory_item_max_quantity[arrayVal] = maxQuantity;
}

int CGUI_Scene2D::IncreaseInventoryItemCount(std::string arrayindex, int incrementValue)
{
	std::cout << "CHECKING INVENTORY";
	int ind = 0;
	bool found = false;
	for (int i = 0; i < inventory_size; i++)
	{
		if (arrayindex == inventory_item_name_list[i])
		{
			ind = i;
			found = true;
			break;
		}
			
	}
	if (found == true)
	{
		inventory_item_quantity[ind] += incrementValue;
		if (inventory_item_quantity[ind] >= inventory_item_max_quantity[ind])
		{
			cout << "RESOURCE FULL!";
			float difference = inventory_item_quantity[ind] - inventory_item_max_quantity[ind];
			inventory_item_quantity[ind] = inventory_item_max_quantity[ind];
			return difference;
		}
		else return 0;
	}
	else
	{
		for (int u = 0; u < inventory_size; u++)
		{
			if (inventory_item_name_list[u].find("empty") != string::npos)
			{
				inventory_item_quantity[u] = incrementValue;
				inventory_item_name_list[u] = arrayindex;
				return 0;
			}

		}
		cout << "INVENTORY FULL!";
		{
			return incrementValue;
		}
	}
}
void CGUI_Scene2D::DecreaseInventoryItemCount(std::string arrayindex, int decrementValue)
{
	int ind = 0;
	for (int i = 0; i < inventory_size; i++)
	{
		if (arrayindex == inventory_item_name_list[i])
		{
			ind = i;
			break;
		}

	}
	inventory_item_quantity[ind] -= decrementValue;
}

void CGUI_Scene2D::InventoryItemSetZero(std::string arrayindex, int incrementValue)
{
	int ind = 0;
	for (int i = 0; i < inventory_size; i++)
	{
		if (arrayindex == inventory_item_name_list[i])
		{
			ind = i;
			break;
		}

	}
	inventory_item_quantity[ind] == 0;
}

bool CGUI_Scene2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();
	cKeyboardController = CKeyboardController::GetInstance();
	// Store the CFPSCounter singleton instance here
	cFPSCounter = CFPSCounter::GetInstance();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(CSettings::GetInstance()->pWindow, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Define the window flags
	window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	//// Show the mouse pointer
	//glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Load the images for buttons
	CImageLoader* il = CImageLoader::GetInstance();

	Add1Button.fileName = "Image\\GUI\\Plus1.png";
	Add1Button.textureID = il->LoadTextureGetID(Add1Button.fileName.c_str(), false);
	
	Add10Button.fileName = "Image\\GUI\\Plus10.png";
	Add10Button.textureID = il->LoadTextureGetID(Add10Button.fileName.c_str(), false);
	
	AddAllButton.fileName = "Image\\GUI\\Minus1.png";
	AddAllButton.textureID = il->LoadTextureGetID(AddAllButton.fileName.c_str(), false);
	//KnightIcon.fileName = "Image\\HumanKnightIcon.png";
	//.textureID = il->LoadTextureGetID(KnightIcon.fileName.c_str(), true);


	// Initialise the cInventoryManager
	cInventoryManager = CInventoryManager::GetInstance();

	// These variables are for IMGUI demo only
	show_demo_window = true;
	show_another_window = true;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	m_fProgressBar = 0.0f;

	return true;
}

bool CGUI_Scene2D::CheckCrafting(int recipeIngredientCount, std::string Ingredients[4], int IngredientRequiredCount[4], std::string ResultantCraft, int CraftedQuantity)
{
	int craftingpossible[4];
	for (int i = 0; i < recipeIngredientCount; i++)
	{
		for (int u = 0; u < inventory_size; u++)
		{
			if (Ingredients[i] == inventory_item_name_list[u] && IngredientRequiredCount[i] <= inventory_item_quantity[u])
			{
				craftingpossible[i] = 1;
				break;
			}
		}
	}
	for (int i = 0; i < recipeIngredientCount; i++)
	{
		if (craftingpossible[i] != 1)
		{
			cout << "NOT ENOUGH MATERIALS";
			return false;
		}
	}
	cout << "ABLE TO CRAFT!";
	for (int i = 0; i < recipeIngredientCount; i++)
	{
		for (int u = 0; u < inventory_size; u++)
		{
			if (Ingredients[i] == inventory_item_name_list[u] && IngredientRequiredCount[i] <= inventory_item_quantity[u])
			{
				inventory_item_quantity[u] = inventory_item_quantity[u] - IngredientRequiredCount[i];
			}
		}
	}
	IncreaseInventoryItemCount(ResultantCraft, CraftedQuantity);
	return true;
}
/**
 @brief Update this instance
 */
void CGUI_Scene2D::Update(const double dElapsedTime)
{
	float level = -1;
	// Calculate the relative scale to our default windows width
	const float relativeScale_x = cSettings->iWindowWidth / 800.0f;
	const float relativeScale_y = cSettings->iWindowHeight / 600.0f;
	float wspace = 0.075f;
	float hspace = 0.15f;
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{
		// Create an invisible window which covers the entire OpenGL window
		ImGui::Begin("Invisible window", NULL, window_flags);
		{
			ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
			ImGui::SetWindowFontScale(1.5f * relativeScale_y);

			// Display the FPS
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %d", cFPSCounter->GetFrameRate());
			std::string recName = inventory_item_name_list[0];

			// Render the inventory items
			if (checkinginventory == true)
			{
				ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, 1));
				ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
				ImGui::SetWindowFontScale(2.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "INVENTORY", cFPSCounter->GetFrameRate());
				ImGui::SetWindowFontScale(1.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "press 1 - 9 to view the item in the slot", cFPSCounter->GetFrameRate());
				ImGuiWindowFlags inventoryWindowFlags =
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoScrollbar;
				for (int i = 0; i < inventory_size; i++)
				{
					level++;
					if (level >= 3)
					{
						level = 0;
						hspace += 0.1f;
						wspace = 0.075f;
					}
					wspace = wspace + 0.15f;
					
					recName = inventory_item_name_list[i];
					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.f, 0.f, 1.0f));  // Set a background color
					const char* c = inventory_item_name_list[i].c_str();
					{
						ImGui::Begin(c, NULL, inventoryWindowFlags);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * wspace, cSettings->iWindowHeight * hspace ));
							ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
							cInventoryItem = cInventoryManager->GetItem(inventory_item_name_list[i]);
							ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(75, 75), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::SameLine();
							ImGui::SetWindowFontScale(1.0f * relativeScale_y);
							if(inventory_item_name_list[i].find("empty") == string::npos)
								ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d", inventory_item_quantity[i], inventory_item_max_quantity[i]);
							else ImGui::TextColored(ImVec4(1, 1, 1, 1), " ", inventory_item_quantity[i], inventory_item_max_quantity[i]);
						}
						ImGui::End();
					}
					ImGui::PopStyleColor();
				}
				if (checkinginventory == true && chestactive == false)
				{
					if (cKeyboardController->IsKeyPressed('1'))
					{
						if (deskey == 1)
						{
							deskey = 0;
							descactive = false;
						}
						else
						{
							deskey = 1;
							descactive = true;
						}

					}
					else if (cKeyboardController->IsKeyPressed('2'))
					{
						if (deskey == 2)
						{
							deskey = 0;
							descactive = false;
						}
						else
						{
							deskey = 2;
							descactive = true;
						}
					}
					else if (cKeyboardController->IsKeyPressed('3'))
					{
						if (deskey == 3)
						{
							deskey = 0;
							descactive = false;
						}
						else
						{
							deskey = 3;
							descactive = true;
						}
					}
					else if (cKeyboardController->IsKeyPressed('4'))
					{
						if (deskey == 4)
						{
							deskey = 0;
							descactive = false;
						}
						else
						{
							deskey = 4;
							descactive = true;
						}
					}
					else if (cKeyboardController->IsKeyPressed('5'))
					{
						if (deskey == 5)
						{
							deskey = 0;
							descactive = false;
						}
						else
						{
							deskey = 5;
							descactive = true;
						}
					}
					else if (cKeyboardController->IsKeyPressed('6'))
					{
						if (deskey == 6)
						{
							deskey = 0;
							descactive = false;
						}
						else
						{
							deskey = 6;
							descactive = true;
						}
					}
					else if (cKeyboardController->IsKeyPressed('7'))
					{
						if (deskey == 7)
						{
							deskey = 0;
							descactive = false;
						}
						else
						{
							deskey = 7;
							descactive = true;
						}
					}
					else if (cKeyboardController->IsKeyPressed('8'))
					{
						if (deskey == 8)
						{
							deskey = 0;
							descactive = false;
						}
						else
						{
							deskey = 8;
							descactive = true;
						}
					}
					else if (cKeyboardController->IsKeyPressed('9'))
					{
						if (deskey == 9)
						{
							deskey = 0;
							descactive = false;
						}
						else
						{
							deskey = 9;
							descactive = true;
						}
					}
				}
			}

			if (crafting == true && recipeactive == false)
			{
				if (cKeyboardController->IsKeyPressed('0'))
				{
					reckey = 0;
					recipeactive = true;
				}
				else if (cKeyboardController->IsKeyPressed('1'))
				{
					reckey = 1;
					recipeactive = true;
				}
				else if (cKeyboardController->IsKeyPressed('2'))
				{
					reckey = 2;
					recipeactive = true;
				}
				else if (cKeyboardController->IsKeyPressed('3'))
				{
					reckey = 3;
					recipeactive = true;
				}
				else if (cKeyboardController->IsKeyPressed('4'))
				{
					reckey = 4;
					recipeactive = true;
				}

			}
			if (crafting == true && recipeactive == false)
			{
		
				ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, cSettings->iWindowHeight / 6));
				ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
				ImGui::SetWindowFontScale(2.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "CRAFTING", cFPSCounter->GetFrameRate());
				ImGuiWindowFlags inventoryWindowFlags =
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoScrollbar;
				for (int i = 0; i < Crafting_list_size; i++)
				{
					level++;
					if (level >= 3)
					{
						level = 0;
						hspace += 0.1f;
						wspace = 0.0f;
					}
					wspace = wspace + 0.20f;

					recName = Crafting_item_name_list[i];
					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f,0.0f,0.2f, 1.0f));  // Set a background color
					const char* c = Crafting_item_name_list[i].c_str();
					std::string cText = "Press (" + std::to_string(i) + ") to view recipe";
					{
						ImGui::Begin(c, NULL, inventoryWindowFlags);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * wspace, cSettings->iWindowHeight * hspace));
							ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
							cInventoryItem = cInventoryManager->GetItem(Crafting_item_name_list[i]);
							ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(50, 50), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::SameLine();
							ImGui::SetWindowFontScale(1.0f * relativeScale_y);
							ImGui::TextColored(ImVec4(1, 1, 0, 1), cText.c_str(), cFPSCounter->GetFrameRate());
						}
						ImGui::End();
					}
					ImGui::PopStyleColor();
				}
			}
			else if (crafting == true && recipeactive == true)
			{
				std::string seltext = "RECIPE SELECTED: " + Crafting_item_name_list[reckey];
				ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, cSettings->iWindowHeight / 6));
				ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
				ImGui::SetWindowFontScale(2.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), seltext.c_str());

				ImGuiWindowFlags inventoryWindowFlags =
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoScrollbar;
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f,0.0f,0.2f, 1.0f));  // Set a background color
				if (reckey == 0)
				{
					const char* c = Crafting_item_name_list[reckey].c_str();
					{
						ImGui::Begin(c, NULL, inventoryWindowFlags);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.3f, cSettings->iWindowHeight * 0.3f));
							ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
							cInventoryItem = cInventoryManager->GetItem(Crafting_item_name_list[reckey]);
							ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::SameLine();
							ImGui::SetWindowFontScale(1.5f * relativeScale_y);
							ImGui::TextColored(ImVec4(1, 1, 0, 1), "REQUIREMENTS\n25 Scrap Metal\n25 Wood\nMakes: 25\nPress C to craft!\nPress R to return!");
						}
						ImGui::End();
					}
					ImGui::PopStyleColor();

					if (cKeyboardController->IsKeyPressed('C'))
					{
						std::string requirementsarray[2] = { "Scrap Metal", "Hard wood" };
						int requirementsCount[2] = { 25, 25 };
						CheckCrafting(2, requirementsarray, requirementsCount, Crafting_item_name_list[reckey], 25);
					}
				}
				if (reckey == 1)
				{
					const char* c = Crafting_item_name_list[reckey].c_str();
					{
						ImGui::Begin(c, NULL, inventoryWindowFlags);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.3f, cSettings->iWindowHeight * 0.3f));
							ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
							cInventoryItem = cInventoryManager->GetItem(Crafting_item_name_list[reckey]);
							ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::SameLine();
							ImGui::SetWindowFontScale(1.5f * relativeScale_y);
							ImGui::TextColored(ImVec4(1, 1, 0, 1), "REQUIREMENTS\n15 Scrap Metal\n10 Wood\nMakes: 25\nPress C to craft!\nPress R to return!");
						}
						ImGui::End();
					}
					ImGui::PopStyleColor();

					if (cKeyboardController->IsKeyPressed('C'))
					{
						std::string requirementsarray[2] = { "Scrap Metal", "Hard wood" };
						int requirementsCount[2] = { 15, 10 };
						CheckCrafting(2, requirementsarray, requirementsCount, Crafting_item_name_list[reckey], 25);
					}
				}
				if (reckey == 2)
				{
					const char* c = Crafting_item_name_list[reckey].c_str();
					{
						ImGui::Begin(c, NULL, inventoryWindowFlags);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.3f, cSettings->iWindowHeight * 0.3f));
							ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
							cInventoryItem = cInventoryManager->GetItem(Crafting_item_name_list[reckey]);
							ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::SameLine();
							ImGui::SetWindowFontScale(1.5f * relativeScale_y);
							ImGui::TextColored(ImVec4(1, 1, 0, 1), "REQUIREMENTS\n15 Stone\n10 Wood\nMakes: 15\nPress C to craft!\nPress R to return!");
						}
						ImGui::End();
					}
					ImGui::PopStyleColor();
					if (cKeyboardController->IsKeyPressed('C'))
					{
						std::string requirementsarray[2] = { "Stone Ore", "Hard wood" };
						int requirementsCount[2] = { 15, 10 };
						CheckCrafting(2, requirementsarray, requirementsCount, Crafting_item_name_list[reckey], 15);
					}
				}
				if (reckey == 3)
				{
					const char* c = Crafting_item_name_list[reckey].c_str();
					{
						ImGui::Begin(c, NULL, inventoryWindowFlags);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.3f, cSettings->iWindowHeight * 0.3f));
							ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
							cInventoryItem = cInventoryManager->GetItem(Crafting_item_name_list[reckey]);
							ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::SameLine();
							ImGui::SetWindowFontScale(1.5f * relativeScale_y);
							ImGui::TextColored(ImVec4(1, 1, 0, 1), "REQUIREMENTS\n5 Wood\n15 Fabric\nMakes: 10\nPress C to craft!\nPress R to return!");
						}
						ImGui::End();
					}
					ImGui::PopStyleColor();
					if (cKeyboardController->IsKeyPressed('C'))
					{
						std::string requirementsarray[2] = { "Fabric", "Hard wood" };
						int requirementsCount[2] = { 15, 5 };
						CheckCrafting(2, requirementsarray, requirementsCount, Crafting_item_name_list[reckey], 10);
					}
				}
				if (reckey == 4)
				{
					const char* c = Crafting_item_name_list[reckey].c_str();
					{
						ImGui::Begin(c, NULL, inventoryWindowFlags);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.3f, cSettings->iWindowHeight * 0.3f));
							ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
							cInventoryItem = cInventoryManager->GetItem(Crafting_item_name_list[reckey]);
							ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::SameLine();
							ImGui::SetWindowFontScale(1.5f * relativeScale_y);
							ImGui::TextColored(ImVec4(1, 1, 0, 1), "REQUIREMENTS\n5 Wood\n5 bandages\nMakes: 1\nPress C to craft!\nPress R to return!");
						}
						ImGui::End();
					}
					ImGui::PopStyleColor();
					if (cKeyboardController->IsKeyPressed('C'))  
					{
						std::string requirementsarray[2] = { "Hard wood", "Bandage" };
						int requirementsCount[2] = { 5, 5 };
						CheckCrafting(2, requirementsarray, requirementsCount, Crafting_item_name_list[reckey], 1);
					}
				}
				if (cKeyboardController->IsKeyPressed('R'))
				{
					recipeactive = false;
					crafting = true;
				}
			}
			else if (descactive == true && checkinginventory == true)
			{
				std::string description = "--Empty--";
				std::string itemname = inventory_item_name_list[deskey - 1];
				const char* c = inventory_item_name_list[deskey - 1].c_str();

				if (itemname == "Hard wood")
				{
					description = "A piece of lumber, cut\ndown from a tree. You \ncan use this to build and \ncraft stuff.";
				}
				else if (itemname == "Stone Ore")
				{
					description = "A hard rock you picked up\n You can use this to build \nand craft stuff.";
				}
				else if (itemname == "Scrap Metal")
				{
					description = "Some pieces of metal junk\nIt contains spare parts, \nbolts, nuts, screws, nails\nall sorts of metal that you\ncan use to craft stuff with";
				}
				else if (itemname == "Stone Ore")
				{
					description = "A hard rock you picked up\n You can use this to build and \ncraft stuff.";
				}
				else if (itemname == "Pistol Bullets")
				{
					description = "Ammo used by light guns\nlike pistols and sub machine \nguns. litle damage but\neasy to find and craft";
				}
				else if (itemname == "Rifle Bullets")
				{
					description = "The stronger caliber\nAmmo used by bigger guns\nlike rifles and snipers\nhigh damage but\nsignificantly harder to\nfind and craft";
				}
				else if (itemname == "Rifle")
				{
					description = "High damage fully\nautomatic 5.56x45 rifle\nused in military purposes\nuses rifle ammo";
				}
				else if (itemname == "Pistol")
				{
					description = "Low damage semi\nautomatic 9x19 pistol\nused in the police force\nand for civilized use\nuses pistol ammo";
				}
				else if (itemname == "Medkit")
				{
					description = "A fully kitted medkit\ncontaining various medical\nsupplies, including disposable\nsyringes, anti inflamation\ntablets, and surgical tools\n Heals your player\nto max health.";
				}
				else if (itemname == "Bandage")
				{
					description = "An antiseptic bandage used\n for medical purposes\nHeals a small amount\nof health";
				}
				else if (itemname == "Fabric")
				{
					description = "A cloth like material crafted\nfrom fiber and hemp.\nused to craft all sorts\nof items";
				}

				if (itemname.find("empty") == string::npos)
				{
					ImGuiWindowFlags inventoryDesc =
						ImGuiWindowFlags_AlwaysAutoResize |
						ImGuiWindowFlags_NoTitleBar |
						ImGuiWindowFlags_NoResize |
						ImGuiWindowFlags_NoCollapse |
						ImGuiWindowFlags_NoScrollbar;
					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f,0.0f,0.2f, 1.0f));  // Set a background color
					{
						ImGui::Begin("M", NULL, inventoryDesc);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.67f, cSettings->iWindowHeight * 0.2));
							ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
							ImGui::SetWindowFontScale(1.5f * relativeScale_y);
							ImGui::TextColored(ImVec4(1, 1, 0, 1), itemname.c_str(), cFPSCounter->GetFrameRate());
							cInventoryItem = cInventoryManager->GetItem(inventory_item_name_list[deskey - 1]);
							ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::SetWindowFontScale(1.0f * relativeScale_y);
							ImGui::TextColored(ImVec4(1, 1, 0, 1), description.c_str(), cFPSCounter->GetFrameRate());
						}
						ImGui::End();
					}
					ImGui::PopStyleColor();
				}
			}
			else if (chestactive == true)
			{
			ImGuiWindowFlags chestbutton =
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoScrollbar;
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.f, 0.f, 1.0f));  // Set a background color
			ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * 0.70,
				CSettings::GetInstance()->iWindowHeight / 5.0));				// Set the top-left of the window at (10,10)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 0.2f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 0.f, 0.f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.2f, 0.2f, 1.f));
			// Add codes for Start button here
			if (ImGui::ImageButton((ImTextureID)Add1Button.textureID,
				ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
			{
				// Reset the CKeyboardController
				CKeyboardController::GetInstance()->Reset();

				// Load the menu state
				//cout << "Loading PlayGameState" << endl;
				cout << "ADDING 1";
			}
			if (ImGui::ImageButton((ImTextureID)Add10Button.textureID,
				ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
			{
				// Reset the CKeyboardController
				CKeyboardController::GetInstance()->Reset();

				// Load the menu state
				//cout << "Loading PlayGameState" << endl;
				cout << "ADDING 10";
			}
			if (ImGui::ImageButton((ImTextureID)AddAllButton.textureID,
				ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
			{
				// Reset the CKeyboardController
				CKeyboardController::GetInstance()->Reset();

				// Load the menu state
				//cout << "Loading PlayGameState" << endl;
				cout << "ADDING ALL";
			}
			ImGui::PopStyleColor(4);
			std::string selecttext = "";
			float hhspace = 0.65f;
			float wwspace = 0.1f;
			if(selectinventory == true) selecttext = "Selected item : " + inventory_item_name_list[chestkey - 1];
			else if (selectinventory == false) selecttext = "Selected item : " + chest_item_name_list[chestkey - 1];
			level = 0; 
				ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, cSettings->iWindowHeight / 6.55));
				ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
				ImGui::SetWindowFontScale(2.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "CHEST INVENTORY", cFPSCounter->GetFrameRate());
				ImGui::SetWindowFontScale(1.0f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "press C to change between chest and inventory selection", cFPSCounter->GetFrameRate());
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "press 1 - 9 to select item in chest / inventory", cFPSCounter->GetFrameRate());
				if(selectinventory == true) ImGui::TextColored(ImVec4(1, 1, 0, 1), "Currently selected: Inventory", cFPSCounter->GetFrameRate());
				else ImGui::TextColored(ImVec4(1, 1, 0, 1), "Currently selected: Chest", cFPSCounter->GetFrameRate());
				if(chestkey != 0) ImGui::TextColored(ImVec4(1, 1, 0, 1), selecttext.c_str(), cFPSCounter->GetFrameRate());
				ImGuiWindowFlags inventoryWindowFlags =
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoScrollbar;

				for (int i = 0; i < Chest_size; i++)
				{
					level++;
					if (level >= 5)
					{
						level = 0;
						hhspace += 0.1f;
						wwspace = 0.1f;
					}
					wwspace = wwspace + 0.15f;

					recName = chest_item_name_list[i];
					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.f, 0.f, 1.0f));  // Set a background color
					const char* c = chest_item_name_list[i].c_str();
					{
						ImGui::Begin(c, NULL, inventoryWindowFlags);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * wwspace, cSettings->iWindowHeight * hhspace));
							ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
							cInventoryItem = cInventoryManager->GetItem(chest_item_name_list[i]);
							ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(75, 75), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::SameLine();
							ImGui::SetWindowFontScale(1.0f * relativeScale_y);
							if (chest_item_name_list[i].find("empty") == string::npos)
								ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d", chest_item_quantity[i], chest_item_max_quantity[i]);
							else ImGui::TextColored(ImVec4(1, 1, 1, 1), " ", chest_item_quantity[i], chest_item_max_quantity[i]);
						}
						ImGui::End();
					}
					ImGui::PopStyleColor();
				}
		
				if (cKeyboardController->IsKeyPressed('C'))
				{
					std::cout << selectinventory;
					if (selectinventory == true)
					{
						selectinventory = false;
					}
					else if (selectinventory == false)
					{
						selectinventory = true;
					}
				}
				if (cKeyboardController->IsKeyPressed('1')) chestkey = 1;
				else if (cKeyboardController->IsKeyPressed('2')) chestkey = 2;
				else if (cKeyboardController->IsKeyPressed('3')) chestkey = 3;
				else if (cKeyboardController->IsKeyPressed('4')) chestkey = 4;
				else if (cKeyboardController->IsKeyPressed('5')) chestkey = 5;
				else if (cKeyboardController->IsKeyPressed('6')) chestkey = 6;
				else if (cKeyboardController->IsKeyPressed('7')) chestkey = 7;
				else if (cKeyboardController->IsKeyPressed('8')) chestkey = 8;
				else if (cKeyboardController->IsKeyPressed('9')) chestkey = 9;

				
			}
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f,0.0f,0.2f, 1.0f));  // Set a background color
			ImGuiWindowFlags topRightWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoScrollbar;
			{
				// Render the Lives
				ImGui::Begin("Lives", NULL, topRightWindowFlags);
				{
					ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.85f, cSettings->iWindowHeight * 0.03f));
					ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));
					cInventoryItem = cInventoryManager->GetItem("Lives");
					ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(cInventoryItem->vec2Size.x * relativeScale_x, cInventoryItem->vec2Size.y * relativeScale_y), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::SameLine();
					ImGui::SetWindowFontScale(1.5f * relativeScale_y);
					ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d / %d", cInventoryItem->GetCount(), cInventoryItem->GetMaxCount());
				}
				ImGui::End();
			}
			ImGui::PopStyleColor();
		}
		ImGui::End();
	}
	ImGui::EndFrame();
}


/**
 @brief Set up the OpenGL display environment before rendering
 */
void CGUI_Scene2D::PreRender(void)
{
}

/**
 @brief Render this instance
 */
void CGUI_Scene2D::Render(void)
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CGUI_Scene2D::PostRender(void)
{
}

void CGUI_Scene2D::Destroy(void)
{
	if (cInventoryManager)
	{
		cInventoryManager->Destroy();
		cInventoryManager = NULL;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// We won't delete this since it was created elsewhere
	cSettings = NULL;
}

