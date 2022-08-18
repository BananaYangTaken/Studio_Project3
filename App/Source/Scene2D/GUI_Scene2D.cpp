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
#include "Player2D_V2.h"
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
			for (int u = 0; u < inventory_size; u++)
			{
				if (inventory_item_name_list[u].find("empty") != string::npos)
				{
					inventory_item_quantity[u] = incrementValue - difference;
					inventory_item_name_list[u] = arrayindex;
					return 0;
				}

			}

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
	if (inventory_item_quantity[ind] <= 0)
	{
		inventory_item_quantity[ind] = 0;
	}
}
void CGUI_Scene2D::checkforzero()
{
	for (int i = 0; i < inventory_size; i++)
	{
		std::string emptyi = "empty" + std::to_string(i);
		if (inventory_item_quantity[i] <= 0 && inventory_item_name_list[i] != "empty" + i)
		{
			std::string emptyi = "empty" + std::to_string(i);
			inventory_item_name_list[i] = emptyi;
		}
	}
}
void CGUI_Scene2D::OpenCrate(std::string size)
{

	if (size == "Large")
	{

		for (int i = 0; i < Chest_size; i++)
		{
			int randitem = rand() % 50 + 1;
			if (randitem == 1)
			{
				crate_item_name_list[i] = "Bandage";
				int rval = rand() % 3 + 1;
				crate_item_quantity[i] = rval;
			}
			else if (randitem == 2)
			{
				crate_item_name_list[i] = "Scrap Metal";
				int rval = rand() % 4 + 1;
				crate_item_quantity[i] = rval;
			}
			else if (randitem == 3)
			{
				crate_item_name_list[i] = "Hard wood";
				int rval = rand() % 4 + 2;
				crate_item_quantity[i] = rval;
			}
			else if (randitem == 4)
			{
				crate_item_name_list[i] = "Stone Ore";
				int rval = rand() % 4 + 2;
				crate_item_quantity[i] = rval;
			}
			else if (randitem == 5)
			{
				crate_item_name_list[i] = "Medkit";
				int rval = 1;
				crate_item_quantity[i] = rval;
			}
			else if (randitem == 6)
			{
				crate_item_name_list[i] = "Fabric";
				int rval = rand() % 3 + 1;
				crate_item_quantity[i] = rval;
			}
			else crate_item_name_list[i] = "empty" + std::to_string(i);
		}
	}
}
void CGUI_Scene2D::InventoryItemSetZero(std::string arrayindex)
{
	int ind = 0;
	for (int i = 0; i < inventory_size; i++)
	{
		std::cout << inventory_item_name_list[i];
		if (arrayindex == inventory_item_name_list[i])
		{
			inventory_item_name_list[i] = "empty" + (i + 1);
			ind = i;
			break;
		}

	}
	inventory_item_quantity[ind] == 0;
}

void CGUI_Scene2D::ChestItemSetZero(std::string arrayindex)
{
	for (int i = 0; i < inventory_size; i++)
	{
		std::string emptyi = "empty" + std::to_string(i);
		if (chest_item_quantity[i] <= 0 && chest_item_name_list[i] != "empty" + i)
		{
			std::string emptyi = "empty" + std::to_string(i);
			chest_item_name_list[i] = emptyi;
		}
	}
}
void CGUI_Scene2D::decreaseChestQuantity(std::string arrayindex, int decrementValue)
{
	int ind = 0;
	for (int i = 0; i < Chest_size; i++)
	{
		if (arrayindex == chest_item_name_list[i])
		{
			ind = i;
			break;
		}

	}
	chest_item_quantity[ind] -= decrementValue;
	if (chest_item_quantity[ind] <= 0)
	{
		chest_item_quantity[ind] = 0;
		ChestItemSetZero(chest_item_name_list[ind]);
	}
}
int CGUI_Scene2D::TransferToChest(std::string itemName, int quantity)
{
	int ind = 0;
	bool found = false;
	for (int i = 0; i < Chest_size; i++)
	{
		if (itemName == chest_item_name_list[i])
		{
			ind = i;
			found = true;
			break;
		}

	}
	if (found == true)
	{
		
		if (chest_item_quantity[ind] >= chest_item_max_quantity[ind])
		{
			cout << "RESOURCE FULL!";
			float difference = chest_item_quantity[ind] - chest_item_max_quantity[ind];
			chest_item_quantity[ind] = chest_item_max_quantity[ind];

			return difference;
		}
		else if (inventory_item_quantity[ind] < quantity)
		{
			cout << "Not enough to init full transfer";
			chest_item_quantity[ind] += inventory_item_quantity[ind];
			DecreaseInventoryItemCount(itemName, quantity);
		}
		else
		{
			chest_item_quantity[ind] += quantity;
			DecreaseInventoryItemCount(itemName, quantity);
			return 0;
		}
	}
	else
	{
		for (int u = 0; u < inventory_size; u++)
		{
			if (chest_item_name_list[u].find("empty") != string::npos)
			{
				if (inventory_item_quantity[ind] <= quantity)
				{
					cout << "Not enough to init full transfer";
					chest_item_quantity[ind] += inventory_item_quantity[ind];
					DecreaseInventoryItemCount(itemName, quantity);
					return 0;
				}
				chest_item_quantity[u] = quantity;
				chest_item_name_list[u] = itemName;
				DecreaseInventoryItemCount(itemName, quantity);
				return 0;
			}


		}
		cout << "INVENTORY FULL!";
		{
			return quantity;
		}
	}

}
int CGUI_Scene2D::TransferTohand(std::string itemName, int quantity)
{
	cout << itemName << endl;
	int ind = 0;
	bool found = false;
	for (int i = 0; i < inventory_size; i++)
	{
		if (itemName == inventory_item_name_list[i])
		{
			ind = i;
			found = true;
			break;
		}

	}
	if (found == true)
	{
		cout << "FOUND!" << endl;
		cout << ind << endl;
		if (inventory_item_quantity[ind] >= inventory_item_max_quantity[ind])
		{
			cout << "RESOURCE FULL!";
			float difference = inventory_item_quantity[ind] - inventory_item_max_quantity[ind];
			inventory_item_quantity[ind] = inventory_item_max_quantity[ind];
			return difference;
		}
		else if (chest_item_quantity[ind] < quantity)
		{
			cout << "Not enough to init full transfer";
			IncreaseInventoryItemCount(inventory_item_name_list[ind], chest_item_quantity[ind]);
			decreaseChestQuantity(itemName, quantity);
		}
		else
		{
			IncreaseInventoryItemCount(inventory_item_name_list[ind], quantity);
			decreaseChestQuantity(itemName, quantity);
			return 0;
		}
		
	}
	else
	{
		cout << "UNFOUND!" << endl;
		for (int u = 0; u < inventory_size; u++)
		{
			if (inventory_item_name_list[u].find("empty") != string::npos)
			{
				inventory_item_name_list[u] = itemName;
				if (chest_item_quantity[ind] <= quantity)
				{
					cout << "Not enough to init full transfer";
					inventory_item_quantity[ind] += chest_item_quantity[ind];
					decreaseChestQuantity(itemName, quantity);
					return 0;
				}
				inventory_item_quantity[u] += quantity;
				decreaseChestQuantity(itemName, quantity);
				return 0;
			}


		}
		cout << "INVENTORY FULL!";
		{
			return quantity;
		}
	}

}
bool CGUI_Scene2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();
	cSoundController = CSoundController::GetInstance();
	cMap2D = CMap2D::GetInstance();
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
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	//// Show the mouse pointer
	//glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Load the images for buttons
	il = CImageLoader::GetInstance();

	Add1Button.fileName = "Image\\GUI\\Plus1.png";
	Add1Button.textureID = il->LoadTextureGetID(Add1Button.fileName.c_str(), false);
	
	Add10Button.fileName = "Image\\GUI\\Plus10.png";
	Add10Button.textureID = il->LoadTextureGetID(Add10Button.fileName.c_str(), false);
	
	AddAllButton.fileName = "Image\\GUI\\Minus1.png";
	AddAllButton.textureID = il->LoadTextureGetID(AddAllButton.fileName.c_str(), false);

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

void CGUI_Scene2D::PreloadInventoryTextures()
{
	for (int i = 0; i < inventory_size; i++)
	{
		newInventorybutton.fileName = "Image\\BlankBoxButton.tga";
		newInventorybutton.textureID = il->LoadTextureGetID(newInventorybutton.fileName.c_str(), false);
		
		newDropbutton.fileName = "Image\\GUI\\Discard.png";
		newDropbutton.textureID = il->LoadTextureGetID(newDropbutton.fileName.c_str(), false);
		newChestbutton.fileName = "Image\\BlankBoxButton.tga";
		newChestbutton.textureID = il->LoadTextureGetID(newChestbutton.fileName.c_str(), false);

		newLootButton.fileName = "Image\\BlankBoxButton.tga";
		newChestbutton.textureID = il->LoadTextureGetID(newLootButton.fileName.c_str(), false);

		newDescriptionButton.fileName = "Image\\GUI\\Dropbutton.png";
		newDescriptionButton.textureID = il->LoadTextureGetID(newDescriptionButton.fileName.c_str(), false);
	}
}
void CGUI_Scene2D::dropitem(int key)
{
	cout << "ITEMNAME: " + inventory_item_name_list[key];
	if (inventory_item_name_list[key] == "Bandage")
		cMap2D->SetMapInfo(CPlayer2D_V2::GetInstance()->vec2Index.x, CPlayer2D_V2::GetInstance()->vec2Index.y, 30);
	if (inventory_item_name_list[key] == "Blueprint")
		cMap2D->SetMapInfo(CPlayer2D_V2::GetInstance()->vec2Index.y, CPlayer2D_V2::GetInstance()->vec2Index.x, 32);
	if (inventory_item_name_list[key] == "Medkit")
		cMap2D->SetMapInfo(CPlayer2D_V2::GetInstance()->vec2Index.x, CPlayer2D_V2::GetInstance()->vec2Index.y, 35);
	if (inventory_item_name_list[key] == "Pistol Bullets")
		cMap2D->SetMapInfo(CPlayer2D_V2::GetInstance()->vec2Index.x, CPlayer2D_V2::GetInstance()->vec2Index.y, 36);
	if (inventory_item_name_list[key] == "Pistol")
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 37);
	if (inventory_item_name_list[key] == "Rifle Bullets")
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 38);
	if (inventory_item_name_list[key] == "Rifle")
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 39);
	if (inventory_item_name_list[key] == "Scrap Metal")
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 40);
	if (inventory_item_name_list[key] == "Stone Ore")
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 41);
	if (inventory_item_name_list[key] == "Fabric")
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 33);
	if (inventory_item_name_list[key] == "Hard wood")
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 34);
	inventory_item_quantity[key] = 0;

}
void CGUI_Scene2D::SwapItems(int itemindex, int swapindex)
{
	std::string temp = inventory_item_name_list[itemindex];
	inventory_item_name_list[itemindex] = inventory_item_name_list[swapindex];
	inventory_item_name_list[swapindex] = temp;

	int tempp = inventory_item_quantity[itemindex];
	inventory_item_quantity[itemindex] = inventory_item_quantity[swapindex];
	inventory_item_quantity[swapindex] = tempp;

	swapactive = false;
	OGclicked = 0;
}

std::string CGUI_Scene2D::GetCurrentHotbarItem(void)
{
	return inventory_item_name_list[hotbarlevel];
}

void CGUI_Scene2D::Update(const double dElapsedTime)
{
	hotbarlevel = 0;
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
				if (recipeactive != true && crafting != true && chestactive != true)
					ImGui::TextColored(ImVec4(1, 1, 0, 1), "Click on the item in the slot to view its details", cFPSCounter->GetFrameRate());
				ImGuiWindowFlags inventoryWindowFlags =
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoInputs |
					ImGuiWindowFlags_NoBringToFrontOnFocus |
					ImGuiWindowFlags_NoNavFocus |
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
					std::string istr = std::to_string(i + 9);
					std::string ctxt = (inventory_item_name_list[i] + istr);
					const char* c = ctxt.c_str();
					hotbarlevel++;
					if (hotbarlevel <= 3)
						ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.f, 0.3f, 1.0f));  // Set a background color
					else
						ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.f, 0.0f, 1.0f));  // Set a background color
					{
						ImGui::Begin(c, NULL, inventoryWindowFlags);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * wspace, cSettings->iWindowHeight * hspace));
							ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));

							cInventoryItem = cInventoryManager->GetItem(inventory_item_name_list[i]);
							ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(75, 75), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::SameLine();
							ImGui::SetWindowFontScale(1.0f * relativeScale_y);
							if (inventory_item_name_list[i].find("empty") == string::npos)
								ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d", inventory_item_quantity[i], inventory_item_max_quantity[i]);
							else ImGui::TextColored(ImVec4(1, 1, 1, 1), " ", inventory_item_quantity[i], inventory_item_max_quantity[i]);

						}
						ImGui::End();
					}
					ImGui::PopStyleColor();

					///
					if (chestactive == false)
					{
						ImGuiWindowFlags inventoryWindowFlagsButton =
						ImGuiWindowFlags_AlwaysAutoResize |
						ImGuiWindowFlags_NoTitleBar |
						ImGuiWindowFlags_NoResize |

						ImGuiWindowFlags_NoCollapse |
						ImGuiWindowFlags_NoBackground |
						ImGuiWindowFlags_NoScrollbar;

						std::string istr = std::to_string(i + 18);
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.f, 0.3f, 0.1f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.f, 0.5f, 0.1f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.2f, 0.5f, 0.3f));
						{
							if (inventoryloaded == false)
							{
								PreloadInventoryTextures();
								inventoryloaded = true;
							}
							ctxt = (inventory_item_name_list[i] + istr);
							c = ctxt.c_str();
							ImGui::Begin(c, NULL, inventoryWindowFlagsButton);
							{
								ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * wspace, cSettings->iWindowHeight * hspace));
								ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
								if (ImGui::ImageButton((ImTextureID)newInventorybutton.textureID,
									ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(0.0, 0.0)))
								{
									CKeyboardController::GetInstance()->Reset();
									if (swapactive == true)
									{
										SwapItems(OGclicked, i);
									}
									else
									{
										cout << "PRESED";
										if (deskey == i && descactive == true) descactive = false;
										else {
											cout << deskey;
											descactive = true;
											deskey = i;
										}
									}
								}

							}
							ImGui::End();
						}
						ImGui::PopStyleColor(3);
					}
					else if (chestactive == true)
					{
						ImGuiWindowFlags inventoryWindowFlagsButton =
							ImGuiWindowFlags_AlwaysAutoResize |
							ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoCollapse |
							ImGuiWindowFlags_NoBackground |
							ImGuiWindowFlags_NoScrollbar;
						if (inventory_item_name_list[i].find("empty") == string::npos)
						{
							std::string istr = std::to_string(i + 18);
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 0.f));
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 0.f, 0.f, 0.1f));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.2f, 0.2f, 0.3f));
							{
								if (inventoryloaded == false)
								{
									PreloadInventoryTextures();
									inventoryloaded = true;
								}
								ctxt = (inventory_item_name_list[i] + istr);
								c = ctxt.c_str();
								ImGui::Begin(c, NULL, inventoryWindowFlagsButton);
								{
									ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * wspace, cSettings->iWindowHeight * hspace));
									ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
									if (ImGui::ImageButton((ImTextureID)newInventorybutton.textureID,
										ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(0.0, 0.0)))
									{
										CKeyboardController::GetInstance()->Reset();
										chestkey = i;
										selectinventory = true;
										cout << chestkey;

									}
								}
								ImGui::End();
							}
							ImGui::PopStyleColor(3);
						}
						else continue;
					}
				}
			}

			else if (hoveroveritem == true)
			{
				std::string floortxt = "Pesss F to pick up " + std::to_string(flooritemquantity) + " " + flooritemname;
				ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, 1));
				ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
				ImGui::SetWindowFontScale(2.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), floortxt.c_str(), cFPSCounter->GetFrameRate());
			}
			else if (checkinginventory == false && crafting == false && descactive == false && chestactive == false)
			{
				ImGuiWindowFlags hotbarflags =
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoInputs |
					ImGuiWindowFlags_NoBringToFrontOnFocus |
					ImGuiWindowFlags_NoNavFocus |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoScrollbar;
				for (int i = 0; i < 3; i++)
				{
					wspace = wspace + 0.10f;
					recName = inventory_item_name_list[i];
					std::string istr = std::to_string(i + 27);
					std::string ctxt = (inventory_item_name_list[i] + istr);
					const char* c = ctxt.c_str();
					if (i + 1 == hotbarselection)
						ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.4f, 0.f, 0.4f, 1.0f));  // Set a background color
					else
						ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.f, 0.1f, 1.0f));  // Set a background color
					{
						ImGui::Begin(c, NULL, hotbarflags);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * wspace, cSettings->iWindowHeight * 0.8f));
							ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));

							cInventoryItem = cInventoryManager->GetItem(inventory_item_name_list[i]);
							ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(75, 75), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::SameLine();
							ImGui::SetWindowFontScale(1.0f * relativeScale_y);
							if (inventory_item_name_list[i].find("empty") == string::npos)
								ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d", inventory_item_quantity[i], inventory_item_max_quantity[i]);
							else ImGui::TextColored(ImVec4(1, 1, 1, 1), " ", inventory_item_quantity[i], inventory_item_max_quantity[i]);

						}
						ImGui::End();
					}
					ImGui::PopStyleColor();
					if (cKeyboardController->IsKeyPressed('3'))
					{
						hotbarselection = 3;
					}
					else if (cKeyboardController->IsKeyPressed('1'))
					{
						hotbarselection = 1;
					}
					else if (cKeyboardController->IsKeyPressed('2'))
					{
						hotbarselection = 2;
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
				wspace = 0.0f;
				hspace = 0.5f;
				ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, cSettings->iWindowHeight / 3));
				ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
				ImGui::SetWindowFontScale(2.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Crafting recipes", cFPSCounter->GetFrameRate());
				ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, 1));
				ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
				ImGui::SetWindowFontScale(2.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), " ", cFPSCounter->GetFrameRate());
				ImGuiWindowFlags inventoryWindowFlags =
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoBringToFrontOnFocus |
					ImGuiWindowFlags_NoNavFocus |
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
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.0f, 0.2f, 1.0f));  // Set a background color
				std::string ctxt = (Crafting_item_name_list[i] + "5");
				const char* c = ctxt.c_str();
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
				ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, cSettings->iWindowHeight / 3));
				ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
				ImGui::SetWindowFontScale(2.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), seltext.c_str());

					ImGuiWindowFlags inventoryWindowFlags =
						ImGuiWindowFlags_AlwaysAutoResize |
						ImGuiWindowFlags_NoTitleBar |
						ImGuiWindowFlags_NoResize |
						ImGuiWindowFlags_NoBringToFrontOnFocus |
						ImGuiWindowFlags_NoNavFocus |
						ImGuiWindowFlags_NoCollapse |
						ImGuiWindowFlags_NoScrollbar;
					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.0f, 0.2f, 1.0f));  // Set a background color

				std::string ctxt = (Crafting_item_name_list[reckey] + "4");
				const char* c = ctxt.c_str();
				{
					ImGui::Begin(c, NULL, inventoryWindowFlags);
					{
						ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.3f, cSettings->iWindowHeight * 0.6f));
						ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
						cInventoryItem = cInventoryManager->GetItem(Crafting_item_name_list[reckey]);
						ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
						ImGui::SameLine();
						ImGui::SetWindowFontScale(1.5f * relativeScale_y);
						ImGui::TextColored(ImVec4(1, 1, 0, 1), description.c_str());
					}
					ImGui::End();
				}
				ImGui::PopStyleColor();
				if (reckey == 0)
				{

					description = "REQUIREMENTS\n25 Scrap Metal\n25 Wood\nMakes: 25\nPress C to craft!\nPress R to return!";
					ctxt = (Crafting_item_name_list[reckey] + "4");

					if (cKeyboardController->IsKeyPressed('C'))
					{
						std::string requirementsarray[2] = { "Scrap Metal", "Hard wood" };
						int requirementsCount[2] = { 25, 25 };
						CheckCrafting(2, requirementsarray, requirementsCount, Crafting_item_name_list[reckey], 25);
					}
				}
				if (reckey == 1)
				{
					description = "REQUIREMENTS\n15 Scrap Metal\n10 Wood\nMakes: 25\nPress C to craft!\nPress R to return!";
					ctxt = (Crafting_item_name_list[reckey] + "4");
					if (cKeyboardController->IsKeyPressed('C'))
					{
						std::string requirementsarray[2] = { "Scrap Metal", "Hard wood" };
						int requirementsCount[2] = { 15, 10 };
						CheckCrafting(2, requirementsarray, requirementsCount, Crafting_item_name_list[reckey], 25);
					}
				}
				if (reckey == 2)
				{
					description = "REQUIREMENTS\n15 Stone\n10 Wood\nMakes: 15\nPress C to craft!\nPress R to return!";
					ctxt = (Crafting_item_name_list[reckey] + "4");

					if (cKeyboardController->IsKeyPressed('C'))
					{
						std::string requirementsarray[2] = { "Stone Ore", "Hard wood" };
						int requirementsCount[2] = { 15, 10 };
						CheckCrafting(2, requirementsarray, requirementsCount, Crafting_item_name_list[reckey], 15);
					}
				}
				if (reckey == 3)
				{
					description = "REQUIREMENTS\n5 Wood\n15 Fabric\nMakes: 10\nPress C to craft!\nPress R to return!";
					ctxt = (Crafting_item_name_list[reckey] + "4");


					if (cKeyboardController->IsKeyPressed('C'))
					{
						std::string requirementsarray[2] = { "Fabric", "Hard wood" };
						int requirementsCount[2] = { 15, 5 };
						CheckCrafting(2, requirementsarray, requirementsCount, Crafting_item_name_list[reckey], 10);
					}
				}
				if (reckey == 4)
				{
					description = "REQUIREMENTS\n5 Wood\n5 bandages\nMakes: 1\nPress C to craft!\nPress R to return!";
					ctxt = (Crafting_item_name_list[reckey] + "4");

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
				std::string itemname = inventory_item_name_list[deskey];
				const char* c = inventory_item_name_list[deskey].c_str();

				if (itemname == "Hard wood")
				{
					description = "A piece of lumber, cut\ndown from a tree. You \ncan use this to build and \ncraft stuff.";
				}
				else if (itemname == "Stone Ore")
				{
					description = "A hard rock you picked up\nYou can use this to build \nand craft stuff.";
				}
				else if (itemname == "Scrap Metal")
				{
					description = "Some pieces of metal junk\nIt contains spare parts, \nbolts, nuts, screws, nails\nall sorts of metal that you\ncan use to craft stuff with";
				}
				else if (itemname == "Pistol Bullets")
				{
					description = "Ammo used by light guns\nlike pistols and sub machine \nguns. little damage but\neasy to find and craft";
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
					description = "A fully kitted medkit\ncontaining various medical\nsupplies, including disposable\nsyringes, anti inflamation\ntablets, and surgical tools\nHeals your player\nto max health.";
				}
				else if (itemname == "Bandage")
				{
					description = "An antiseptic bandage used\nfor medical purposes\nHeals a small amount\nof health";
				}
				else if (itemname == "Fabric")
				{
					description = "A cloth like material crafted\nfrom fiber and hemp.\nused to craft all sorts\nof items";
				}
				else if (itemname == "Blueprint")
				{
					description = "A set of blueprint, likely\nused by the military to\ncraft special weapons.\nCan be used to craft\nstuff with";
				}

					if (itemname.find("empty") == string::npos)
					{
						ImGuiWindowFlags inventoryDesc =
							ImGuiWindowFlags_AlwaysAutoResize |
							ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoBringToFrontOnFocus |
							ImGuiWindowFlags_NoNavFocus |
							ImGuiWindowFlags_NoCollapse |
							ImGuiWindowFlags_NoScrollbar;
						ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.0f, 0.2f, 1.0f));  // Set a background color
						{
							ImGui::Begin("M", NULL, inventoryDesc);
							{
								ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.67f, cSettings->iWindowHeight * 0.2));
								ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
								ImGui::SetWindowFontScale(1.5f * relativeScale_y);
								ImGui::TextColored(ImVec4(1, 1, 0, 1), itemname.c_str(), cFPSCounter->GetFrameRate());
								cInventoryItem = cInventoryManager->GetItem(inventory_item_name_list[deskey]);
								ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
								ImGui::SetWindowFontScale(1.0f * relativeScale_y);
								ImGui::TextColored(ImVec4(1, 1, 0, 1), description.c_str(), cFPSCounter->GetFrameRate());
							}
							ImGui::End();
						}

						ImGuiWindowFlags inventoryDescButtons =
							ImGuiWindowFlags_AlwaysAutoResize |
							ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoCollapse |
							ImGuiWindowFlags_NoScrollbar;
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 1.f, 0.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 1.f, 1.f, 0.2f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 1.2f, 1.2f, 0.5f));
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.67f, cSettings->iWindowHeight * 0.4));
							ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
							if (looting == false)
							{
								if (ImGui::ImageButton((ImTextureID)newDescriptionButton.textureID,
									ImVec2(128, buttonHeight), ImVec2(1.0, 1.0), ImVec2(0.0, 0.0)))
								{
									swapactive = true;
									OGclicked = deskey;
								}
							}
							if (ImGui::ImageButton((ImTextureID)newDropbutton.textureID,
								ImVec2(128, buttonHeight), ImVec2(0.0, 0.0), ImVec2(0.0, 0.0)))
							{
								CKeyboardController::GetInstance()->Reset();
								if (inventory_item_name_list[deskey].find("empty") == string::npos)
								{
									cout << "Dropped " + inventory_item_name_list[deskey];
									dropitem(deskey);
								}
							}
						}
						ImGui::PopStyleColor(4);
					}
				}
			else if (chestactive == true)
			{
				ImGuiWindowFlags chestbutton =
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoNavFocus |
					ImGuiWindowFlags_NoBringToFrontOnFocus |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoScrollbar;
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.f, 0.f, 1.0f));  // Set a background color
				ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * 0.70,CSettings::GetInstance()->iWindowHeight * 0.3));				// Set the top-left of the window at (10,10)
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
				if (selectinventory == true)
				{
					TransferToChest(inventory_item_name_list[chestkey], 1);
				}
				else TransferTohand(chest_item_name_list[chestkey], 1);
			}
			if (ImGui::ImageButton((ImTextureID)Add10Button.textureID,
				ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
			{
				// Reset the CKeyboardController
				CKeyboardController::GetInstance()->Reset();

					// Load the menu state
					//cout << "Loading PlayGameState" << endl;
					if (selectinventory == true)
					{
						TransferToChest(inventory_item_name_list[chestkey], 10);
					}
					else TransferTohand(chest_item_name_list[chestkey], 10);
				}
				if (ImGui::ImageButton((ImTextureID)AddAllButton.textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					if (selectinventory == true)
					{
						TransferToChest(inventory_item_name_list[chestkey], 100);
					}
					else TransferTohand(chest_item_name_list[chestkey], 100);
				}
				ImGui::PopStyleColor(4);
				std::string selecttext = "";
				float hhspace = 0.65f;
				float wwspace = 0.1f;
				if (inventory_item_name_list[chestkey].find("empty") == string::npos)
				{
					if (selectinventory == true) selecttext = "Selected item : " + inventory_item_name_list[chestkey];
				}
				else if (chest_item_name_list[chestkey].find("empty") == string::npos)
				{
					if (selectinventory == false) selecttext = "Selected item : " + chest_item_name_list[chestkey];
				}
				level = 0;
				ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, cSettings->iWindowHeight / 6.55));
				ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
				ImGui::SetWindowFontScale(2.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "CHEST INVENTORY", cFPSCounter->GetFrameRate());
				ImGui::SetWindowFontScale(1.0f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Click on the inventory or chest boxes to select them!", cFPSCounter->GetFrameRate());
				if (selectinventory == true) ImGui::TextColored(ImVec4(1, 1, 0, 1), "Currently selected: Inventory", cFPSCounter->GetFrameRate());
				else ImGui::TextColored(ImVec4(1, 1, 0, 1), "Currently selected: Chest", cFPSCounter->GetFrameRate());
				ImGui::TextColored(ImVec4(1, 1, 0, 1), selecttext.c_str(), cFPSCounter->GetFrameRate());
				ImGuiWindowFlags inventoryWindowFlags =
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoNavFocus |
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
				std::string ctxt = (chest_item_name_list[i] + "45");
				const char* c = ctxt.c_str();
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
				///
				ImGuiWindowFlags chestWindowFlagsButton =
					ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoNavFocus |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoBackground |
					ImGuiWindowFlags_NoScrollbar;

				std::string istr = std::to_string(i + 36);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 0.f, 0.f, 0.1f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.2f, 0.2f, 0.3f));
				{
					if (inventoryloaded == false)
					{
						PreloadInventoryTextures();
						inventoryloaded = true;
					}
					ctxt = (inventory_item_name_list[i] + istr);
					c = ctxt.c_str();
					ImGui::Begin(c, NULL, chestWindowFlagsButton);
					{
						ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * wwspace, cSettings->iWindowHeight * hhspace));
						ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
						if (ImGui::ImageButton((ImTextureID)newChestbutton.textureID,
							ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(0.0, 0.0)))
						{
							CKeyboardController::GetInstance()->Reset();
							chestkey = i;
							cout << chestkey << endl;
							selectinventory = false;

						}
					}
					ImGui::End();
				}
				ImGui::PopStyleColor(3);
			}

			if (cKeyboardController->IsKeyPressed('C'))
			{
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
			if (looting == true)
			{
				int level = 0;
				/*std::string searchtxt = "SEARCHING...";
				issearched = false;
				searchtimer = 0;*/
				searchtimer++;
				float hhspace = 0.65f;
				float wwspace = 0.1f;
				if (searchtimer >= 40)
				{
					searchtxt = "CRATE INVENTORY";

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

						recName = crate_item_name_list[i];
						ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.f, 0.f, 1.0f));  // Set a background color
						std::string istr = std::to_string(i + 45);
						std::string ctxt = (crate_item_name_list[i] + istr);
						const char* c = ctxt.c_str();
						ImGuiWindowFlags LootCrateFlags =
							ImGuiWindowFlags_AlwaysAutoResize |
							ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoCollapse |
							ImGuiWindowFlags_NoScrollbar;
						{
							ImGui::Begin(c, NULL, LootCrateFlags);
							{
								ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * wwspace, cSettings->iWindowHeight * hhspace));
								ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
								cInventoryItem = cInventoryManager->GetItem(crate_item_name_list[i]);
								ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(75, 75), ImVec2(0, 1), ImVec2(1, 0));
								ImGui::SameLine();
								ImGui::SetWindowFontScale(1.0f * relativeScale_y);
								if (crate_item_name_list[i].find("empty") == string::npos)
									ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d", crate_item_quantity[i]);
								else ImGui::TextColored(ImVec4(1, 1, 1, 1), " ");
							}
							ImGui::End();
						}
						ImGui::PopStyleColor();
						if (crate_item_name_list[i].find("empty") == string::npos)
						{
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 0.f));
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 0.f, 0.f, 0.1f));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.2f, 0.2f, 0.3f));
							{
								if (inventoryloaded == false)
								{
									PreloadInventoryTextures();
									inventoryloaded = true;
								}
								istr = std::to_string(i + 63);
								ctxt = (crate_item_name_list[i] + istr);
								c = ctxt.c_str();
								ImGui::Begin(c, NULL, LootCrateFlags);
								{
									ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * wwspace, cSettings->iWindowHeight * hhspace));
									ImGui::SetWindowSize(ImVec2(25.0f, 25.0f));
									if (ImGui::ImageButton((ImTextureID)newLootButton.textureID,
										ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(0.0, 0.0)))
									{
										if (IncreaseInventoryItemCount(crate_item_name_list[i], crate_item_quantity[i]) == 0)
										{
											std::string str = "empty" + std::to_string(i);
											crate_item_name_list[i] = str;
											crate_item_quantity[i] = 0;

											cout << "added item: " << crate_item_name_list[i] << endl;
										}

									}
								}
								ImGui::End();
							}
							ImGui::PopStyleColor(3);
						}
					}
				}
					
				cout << "LOOTING LARGE CRATE";
				if(descactive == false)
					ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, cSettings->iWindowHeight / 2.55));
				else ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, cSettings->iWindowHeight / 3.55));
				ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
				ImGui::SetWindowFontScale(2.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), searchtxt.c_str(), cFPSCounter->GetFrameRate());
				ImGui::SetWindowFontScale(1.0f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Click on the items in the crate to loot them!", cFPSCounter->GetFrameRate());
				/*ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, cSettings->iWindowHeight / 10));
				ImGui::TextColored(ImVec4(1, 1, 0, 1), " ", cFPSCounter->GetFrameRate());*/

				
		}


			// Render Progress Bar
			ImGui::Begin("ProgressBar", NULL, window_flags);
			{
				ImGui::SetWindowPos(ImVec2((float)cSettings->iWindowWidth - 320.0f, 20.0f));
				ImGui::SetWindowSize(ImVec2(300.0f, 60.0f));
				ImVec4 col = ImVec4(0.f, 1.f, 0.f, 1.f);
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);
					col = ImVec4(1.f, 0.f, 0.f, 1.f);
					ImGui::PushStyleColor(ImGuiCol_FrameBg, col);
					m_fProgressBar = float(CPlayer2D_V2::GetInstance()->GetHealth()) / float(CPlayer2D_V2::GetInstance()->GetMaxHealth());
					const char* empty = " ";
						ImGui::ProgressBar(m_fProgressBar, ImVec2(300.0f, 60.0f), empty);
						// Render the Health
						ImGuiWindowFlags topRightWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
							ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoMove |
							ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoCollapse |
							ImGuiWindowFlags_NoBackground |
							ImGuiWindowFlags_NoScrollbar;

						ImGui::Begin("HealthIcon", NULL, topRightWindowFlags);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.8f, cSettings->iWindowHeight * 0.03f));
							ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y)); 
							cInventoryItem = cInventoryManager->GetItem("Health");
							ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(cInventoryItem->vec2Size.x* relativeScale_x, cInventoryItem->vec2Size.y* relativeScale_y), ImVec2(0, 1), ImVec2(1, 0));

						}
						ImGui::End();
						ImGui::Begin("HealthText", NULL, topRightWindowFlags);
						{
							ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.875f, cSettings->iWindowHeight * 0.03f));
							ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));
							ImGui::SetWindowFontScale(1.5f * relativeScale_y);
							ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d / %d", CPlayer2D_V2::GetInstance()->GetHealth(), CPlayer2D_V2::GetInstance()->GetMaxHealth());
						}
						ImGui::End();
					ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			} 
			ImGui::End();
			
		}
		ImGui::End();
		ImGui::EndFrame();
		checkforzero();
	}
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

