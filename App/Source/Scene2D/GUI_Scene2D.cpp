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

void CGUI_Scene2D::IncreaseInventoryItemCount(int arrayVal, int incrementValue)
{
	inventory_item_quantity[arrayVal] = incrementValue;
}
void CGUI_Scene2D::DecreaseInventoryItemCount(int arrayVal, int decrementValue)
{
	inventory_item_quantity[arrayVal] = decrementValue;
}
bool CGUI_Scene2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

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

/**
 @brief Update this instance
 */
void CGUI_Scene2D::Update(const double dElapsedTime)
{
	// Calculate the relative scale to our default windows width
	const float relativeScale_x = cSettings->iWindowWidth / 800.0f;
	const float relativeScale_y = cSettings->iWindowHeight / 600.0f;

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


			// Render the inventory items
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));  // Set a background color
			ImGuiWindowFlags inventoryWindowFlags =
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoScrollbar;
			if (checkinginventory == true)
			{
				for (int i = 0; i < inventory_size; i++)
				{
					const char* c = inventory_item_name_list[1].c_str();
					std::string cText = std::string(c) + ": %d / %d";
					ImGui::Begin(c, NULL, inventoryWindowFlags);
					{
						ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.4f, cSettings->iWindowHeight * 0.2f + 15 * i));
						ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
						cInventoryItem = cInventoryManager->GetItem(inventory_item_name_list[i]);
						ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(cInventoryItem->vec2Size.x * relativeScale_x, cInventoryItem->vec2Size.y * relativeScale_y), ImVec2(0, 1), ImVec2(1, 0));
						ImGui::SameLine();
						ImGui::SetWindowFontScale(1.5f * relativeScale_y);
						ImGui::TextColored(ImVec4(1, 1, 0, 1), cText.c_str(), inventory_item_quantity[i], inventory_item_max_quantity[i]);
					}
					ImGui::End();
					/*ImGui::Begin("Rune", NULL, inventoryWindowFlags);
					{
						ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.01f, cSettings->iWindowHeight * 0.85f));
						ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
						cInventoryItem = cInventoryManager->GetItem("Rune");
						ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(cInventoryItem->vec2Size.x * relativeScale_x, cInventoryItem->vec2Size.y * relativeScale_y), ImVec2(0, 1), ImVec2(1, 0));
						ImGui::SameLine();
						ImGui::SetWindowFontScale(1.5f * relativeScale_y);
						ImGui::TextColored(ImVec4(1, 1, 0, 1), "Rune: %d / %d", cInventoryItem->GetCount(), cInventoryItem->GetMaxCount());
					}
					ImGui::End();*/
				}
			}
			ImGui::PopStyleColor();

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

