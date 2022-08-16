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
	float wspace = 0.2f;
	float hspace = 0.3f;
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
				ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth / 5, cSettings->iWindowHeight / 6));
				ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
				ImGui::SetWindowFontScale(2.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "INVENTORY", cFPSCounter->GetFrameRate());
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
						wspace = 0.2f;
					}
					wspace = wspace + 0.07f;
					
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
					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));  // Set a background color
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
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));  // Set a background color
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
				}
				if (cKeyboardController->IsKeyPressed('R'))
				{
					recipeactive = false;
					crafting = true;
				}
			}
			float buttonWidth = 256;
			float buttonHeight = 128;
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));  // Set a background color
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

