// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "UpgradeState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include Mesh Builder
#include "Primitives/MeshBuilder.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

 // Include shader
#include "RenderControl\shader.h"

// Include CSettings
#include "GameControl/Settings.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"


#include <includes/irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CUpgradeState::CUpgradeState(void)
	//: background(NULL)
{

}

/**
 @brief Destructor
 */
CUpgradeState::~CUpgradeState(void)
{

}

/**
 @brief Init this class instance
 */
bool CUpgradeState::Init(void)
{
	cout << "CUpgradeState::Init()\n" << endl;
	cInventoryManager = CInventoryManager::GetInstance();
	CShaderManager::GetInstance()->Use("Shader2D");
	cGUIScene2D = CGUI_Scene2D::GetInstance();
	cScene2D = CScene2D::GetInstance();
	//CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	// Load the images for buttons
	il = CImageLoader::GetInstance();

	WindowUpgrade[0].fileName = "Image\\GUI\\WindowUp1.png";
	WindowUpgrade[0].textureID = il->LoadTextureGetID(WindowUpgrade[0].fileName.c_str(), false);

	WindowUpgrade[1].fileName = "Image\\GUI\\WindowUp2.png";
	WindowUpgrade[1].textureID = il->LoadTextureGetID(WindowUpgrade[1].fileName.c_str(), false);

	WindowUpgrade[2].fileName = "Image\\GUI\\WindowUp3.png";
	WindowUpgrade[2].textureID = il->LoadTextureGetID(WindowUpgrade[2].fileName.c_str(), false);



	TurretUpgrade[0].fileName = "Image\\GUI\\TurretUp1.png";
	TurretUpgrade[0].textureID = il->LoadTextureGetID(TurretUpgrade[0].fileName.c_str(), false);

	TurretUpgrade[1].fileName = "Image\\GUI\\TurretUp2.png";
	TurretUpgrade[1].textureID = il->LoadTextureGetID(TurretUpgrade[1].fileName.c_str(), false);

	TurretUpgrade[2].fileName = "Image\\GUI\\TurretUp3.png";
	TurretUpgrade[2].textureID = il->LoadTextureGetID(TurretUpgrade[2].fileName.c_str(), false);


	Barbwire[0].fileName = "Image\\GUI\\Barbwire.png";
	Barbwire[0].textureID = il->LoadTextureGetID(Barbwire[0].fileName.c_str(), false);

	Barbwire[1].fileName = "Image\\GUI\\BarbwireDone.png";
	Barbwire[1].textureID = il->LoadTextureGetID(Barbwire[1].fileName.c_str(), false);

	


	cSettings = CSettings::GetInstance();
	cSoundController = CSoundController::GetInstance();
	// Enable the cursor
	if (CSettings::GetInstance()->bDisableMousePointer == true)
		glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	cMap2D = CMap2D::GetInstance();


	return true;
}

void CUpgradeState::Preload(void)
{
	TurretUpgrade[0].textureID = il->LoadTextureGetID(TurretUpgrade[0].fileName.c_str(), false);
	WindowUpgrade[0].textureID = il->LoadTextureGetID(WindowUpgrade[0].fileName.c_str(), false);
	Barbwire[0].textureID = il->LoadTextureGetID(Barbwire[0].fileName.c_str(), false);
}

/**
 @brief Update this class instance
 */
bool CUpgradeState::SearchForRequirements(std::string list[3], int listcount[3])
{
	bool recipehave[3] = {false,false,false};
	int recipecount = 0;
	for (int i = 0; i < 3; i++)
	{
		if (list[i].find("empty") != string::npos) continue;
		else
		{
			recipecount++;
			for (int u = 0; u < cGUIScene2D->inventory_size; u++)
			{
				if (cGUIScene2D->inventory_item_name_list[u] == list[i] && cGUIScene2D->inventory_item_quantity[u] >= listcount[i])
				{
					recipehave[i] = true;
					break;
				}
			}
		}

	}
	for (int t = 0; t < recipecount; t++)
	{
		if (recipehave[t] == false)
		{
			for(int n = 0; n < recipecount; n++)
				cout << recipehave[n] << endl;
			cout << "COULD NOT FIND ALL AVAILABLE MATERIALS" << endl;
			return false;
		}
	}
	for (int i = 0; i < 3; i++)
	{
		if (list[i].find("empty") != string::npos) continue;
		else
		{
			for (int u = 0; u < cGUIScene2D->inventory_size; u++)
			{
				if (cGUIScene2D->inventory_item_name_list[u] == list[i] && cGUIScene2D->inventory_item_quantity[u] >= listcount[i])
				{
					cout << "Decrementing " << listcount[i] << " " << cGUIScene2D->inventory_item_name_list[u] << " from your inventory" << endl;
					cGUIScene2D->DecreaseInventoryItemCount(cGUIScene2D->inventory_item_name_list[u], listcount[i]);
				}
			}
		}
	}
	return true;
}

bool CUpgradeState::Update(const double dElapsedTime)
{
	float buttonWidth = 200;
	float buttonHeight = 100;

	const float relativeScale_x = cSettings->iWindowWidth / 800.0f;
	const float relativeScale_y = cSettings->iWindowHeight / 600.0f;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
		ImGuiWindowFlags window_flags_bg = 0;
		window_flags_bg |= ImGuiWindowFlags_NoTitleBar;
		window_flags_bg |= ImGuiWindowFlags_NoScrollbar;
		//window_flags |= ImGuiWindowFlags_MenuBar;
		//window_flags |= ImGuiWindowFlags_NoMove;
		window_flags_bg |= ImGuiWindowFlags_NoCollapse;
		window_flags_bg |= ImGuiWindowFlags_NoNav;
		window_flags_bg |= ImGuiWindowFlags_NoResize;

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		//window_flags |= ImGuiWindowFlags_MenuBar;
		//window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoNav;
		window_flags |= ImGuiWindowFlags_NoResize;


		ImGuiWindowFlags window_flags_images = 0;
		window_flags_images |= ImGuiWindowFlags_NoTitleBar;
		window_flags_images |= ImGuiWindowFlags_NoScrollbar;
		//window_flags |= ImGuiWindowFlags_MenuBar;
		window_flags_images |= ImGuiWindowFlags_NoCollapse;
		window_flags_images |= ImGuiWindowFlags_NoNav;
		window_flags_images |= ImGuiWindowFlags_NoResize;

		ImGuiWindowFlags window_flags_images_2 = 0;
		window_flags_images_2 |= ImGuiWindowFlags_NoTitleBar;
		window_flags_images_2 |= ImGuiWindowFlags_NoScrollbar;
		//window_flags |= ImGuiWindowFlags_MenuBar;
		window_flags_images_2 |= ImGuiWindowFlags_NoCollapse;
		window_flags_images_2 |= ImGuiWindowFlags_NoNav;
		window_flags_images_2 |= ImGuiWindowFlags_NoResize;

		static float f = 0.0f;
		static int counter = 0;



		
		// Create a window called Main menu
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.f, 0.2f, 1.0f));  // Set a background color
		ImGui::Begin("Main Menu", NULL, window_flags_bg);
		{
			ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.35, CSettings::GetInstance()->iWindowHeight / 4));
			ImGui::SetWindowFontScale(3.5);
			ImGui::TextColored(ImVec4(1, 1, 1, 1), "UPGRADE MENU");


			ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 4.0,CSettings::GetInstance()->iWindowHeight / 4.0));				// Set the top-left of the window at (10,10)
			ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth/2, CSettings::GetInstance()->iWindowHeight/2));

			//Added rounding for nicer effect
			ImGuiStyle& style = ImGui::GetStyle();
			style.FrameRounding = 200.0f;
		}
		ImGui::End();
		ImGui::PopStyleColor();

		CurrentWindowLv = cScene2D->GetWindowUpgradeLvl();
		CurrentTurretLv = cScene2D->GetTurretUpgradeLvl();
		CurrentBarbWireLv = cScene2D->GetBarbwireUpgradeLvl();

		//Window for Window Upgrade Button
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.f, 0.3f, 1.0f));  // Set a background color
			{
				ImGui::Begin("Window Upgrade", NULL, window_flags);
				{
					ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 3, CSettings::GetInstance()->iWindowHeight / 3.2));
					ImGui::SetWindowSize(ImVec2(640.0, 150.0));

					//window upgrade
					{
						ImGui::SetWindowFontScale(1.5f * relativeScale_y);
						ImGui::TextColored(ImVec4(1, 1, 0, 1), "  Material cost:     ");
						ImGui::SameLine();
						if (CurrentWindowLv == 0)
						{
							if (ImGui::ImageButton((ImTextureID)WindowUpgrade[0].textureID,
								ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
							{
								if (SearchForRequirements(itemlist, itemlistcount) == true)
									CurrentWindowLv += 1;
								// Reset the CKeyboardController
								CKeyboardController::GetInstance()->Reset();
							}

						}

						if (CurrentWindowLv == 1)
						{
							if (ImGui::ImageButton((ImTextureID)WindowUpgrade[1].textureID,
								ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
							{
								if (SearchForRequirements(itemlist, itemlistcount) == true)
									CurrentWindowLv += 1;
								// Reset the CKeyboardController
								CKeyboardController::GetInstance()->Reset();
							}
							cMap2D->SetMapInfo(50, 9, 119);
							cMap2D->SetMapInfo(50, 17, 119);
							cMap2D->SetMapInfo(36, 9, 119);
							cMap2D->SetMapInfo(36, 17, 119);
						}
						if (CurrentWindowLv == 2)
						{
							if (ImGui::ImageButton((ImTextureID)WindowUpgrade[2].textureID,
								ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
							{
								// Reset the CKeyboardController
								CKeyboardController::GetInstance()->Reset();
							}
							cMap2D->SetMapInfo(50, 9, 120);
							cMap2D->SetMapInfo(50, 17, 120);
							cMap2D->SetMapInfo(36, 9, 120);
							cMap2D->SetMapInfo(36, 17, 120);
						}
					}
				}
				ImGui::End();
			}
			ImGui::PopStyleColor();


			//Materials
			{
				if (CurrentWindowLv == 0)
				{
					itemlist[0] = "Hard wood";
					itemlist[1] = "Scrap Metal";
					itemlistcount[0] = 100;
					itemlistcount[1] = 100;

				}
				if (CurrentWindowLv == 1)
				{
					itemlist[0] = "Hard wood";
					itemlist[1] = "Scrap Metal";
					itemlist[2] = "Stone Ore";
					itemlistcount[0] = 50;
					itemlistcount[1] = 225;
					itemlistcount[2] = 100;
				}
				if (CurrentWindowLv == 2)
				{
					itemlist[0] = "empty1";
					itemlist[1] = "empty2";
					itemlist[2] = "empty3";
					itemlistcount[0] = 50;
					itemlistcount[1] = 225;
					itemlistcount[2] = 100;
				}
				for (int i = 0; i < 3; i++)
				{
					if (itemlist[i].find("empty") == string::npos)
					{
						ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.f, 0.3f, 1.0f));  // Set a background color
						ImGui::Begin("Window Upgrade 2", NULL, window_flags_images);
						ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 3, CSettings::GetInstance()->iWindowHeight / 2.8));
						ImGui::SetWindowSize(ImVec2(400.0, 100.0));

						std::string itemlistcountstr = std::to_string(itemlistcount[i]);
						cInventoryItem = cInventoryManager->GetItem(itemlist[i]);
						ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(70, 70), ImVec2(0, 1), ImVec2(1, 0));
						ImGui::SetWindowFontScale(1.0f * relativeScale_y);
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(1, 1, 0, 1), itemlistcountstr.c_str());
						ImGui::SameLine();

						ImGui::End();
						ImGui::PopStyleColor();
					}
				}

			}
		}
		
		
	

		//Window for Turret Upgrade Button
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.f, 0.3f, 1.0f));  // Set a background color
			ImGui::Begin("Turret Upgrade", NULL, window_flags);
			{
				ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 3, CSettings::GetInstance()->iWindowHeight / 2.2));
				ImGui::SetWindowSize(ImVec2(640.0, 150.0));

				//Turret upgrade
				{
					ImGui::SetWindowFontScale(1.5f * relativeScale_y);
					ImGui::TextColored(ImVec4(1, 1, 0, 1), "  Material cost:     ");
					ImGui::SameLine();
					if (CurrentTurretLv == 0)
					{
						if (ImGui::ImageButton((ImTextureID)TurretUpgrade[0].textureID,
							ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
						{
							if (SearchForRequirements(Turretitemlist, Turretitemlistcount) == true)
							{
								CurrentTurretLv += 1;
								// Reset the CKeyboardController
								CKeyboardController::GetInstance()->Reset();
							}	
						}
					}
					if (CurrentTurretLv == 1)
					{
						if (ImGui::ImageButton((ImTextureID)TurretUpgrade[1].textureID,
							ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
						{
							if (SearchForRequirements(Turretitemlist, Turretitemlistcount) == true)
							{
								CurrentTurretLv += 1;
								// Reset the CKeyboardController
								CKeyboardController::GetInstance()->Reset();
							}	
						}
					}
					if (CurrentTurretLv == 2)
					{
						if (ImGui::ImageButton((ImTextureID)TurretUpgrade[2].textureID,
							ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
						{
							// Reset the CKeyboardController
							CKeyboardController::GetInstance()->Reset();
						}
					}

				}
			}
			ImGui::End();
			ImGui::PopStyleColor();


			//Materials
			{
				if (CurrentTurretLv == 0)
				{
					Turretitemlist[0] = "Stone Ore";
					Turretitemlist[1] = "Scrap Metal";
					Turretitemlistcount[0] = 150;
					Turretitemlistcount[1] = 100;

				}
				if (CurrentTurretLv == 1)
				{
					Turretitemlist[0] = "Blueprint";
					Turretitemlist[1] = "Scrap Metal";
					Turretitemlist[2] = "Stone Ore";
					Turretitemlistcount[0] = 1;
					Turretitemlistcount[1] = 500;
					Turretitemlistcount[2] = 200;
				}
				if (CurrentTurretLv == 2)
				{
					Turretitemlist[0] = "empty1";
					Turretitemlist[1] = "empty2";
					Turretitemlist[2] = "empty3";
					Turretitemlistcount[0] = 50;
					Turretitemlistcount[1] = 225;
					Turretitemlistcount[2] = 100;
				}
				for (int i = 0; i < 3; i++)
				{
					if (Turretitemlist[i].find("empty") == string::npos)
					{
						ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.f, 0.3f, 1.0f));  // Set a background color
						ImGui::Begin("Turret Upgrade 2", NULL, window_flags_images);
						ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 3, CSettings::GetInstance()->iWindowHeight / 2.0));
						ImGui::SetWindowSize(ImVec2(400.0, 100.0));

						std::string itemlistcountstr = std::to_string(Turretitemlistcount[i]);
						cInventoryItem = cInventoryManager->GetItem(Turretitemlist[i]);
						ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(70, 70), ImVec2(0, 1), ImVec2(1, 0));
						ImGui::SetWindowFontScale(1.0f * relativeScale_y);
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(1, 1, 0, 1), itemlistcountstr.c_str());
						ImGui::SameLine();

						ImGui::End();
						ImGui::PopStyleColor();
					}
				}

			}
		}

		//Window for Barbwire Upgrade Button
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.f, 0.3f, 1.0f));  // Set a background color
			ImGui::Begin("Barbwire", NULL, window_flags);
			{
				ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 3, CSettings::GetInstance()->iWindowHeight / 1.68));
				ImGui::SetWindowSize(ImVec2(640.0, 150.0));

				//Barbwire upgrade
				{
					ImGui::SetWindowFontScale(1.5f * relativeScale_y);
					ImGui::TextColored(ImVec4(1, 1, 0, 1), "  Material cost:     ");
					ImGui::SameLine();
					if (CurrentBarbWireLv == 0)
					{
						if (ImGui::ImageButton((ImTextureID)Barbwire[0].textureID,
							ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
						{
							if (SearchForRequirements(Wireitemlist, Wireitemlistcount) == true)
								CurrentBarbWireLv += 1;
							// Reset the CKeyboardController
							CKeyboardController::GetInstance()->Reset();
						}
					}
					if (CurrentBarbWireLv == 1)
					{
						if (ImGui::ImageButton((ImTextureID)Barbwire[1].textureID,
							ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
						{
							// Reset the CKeyboardController
							CKeyboardController::GetInstance()->Reset();
						}
						for (int x = 5; x < 22; x++)
						{
							cMap2D->SetMapInfo(52, x, 24);
						}

						for (int y = 34; y < 53; y++)
						{
							cMap2D->SetMapInfo(y, 4, 24);
						}

						for (int x = 5; x < 22; x++)
						{
							cMap2D->SetMapInfo(34, x, 24);
						}

						for (int y = 34; y < 53; y++)
						{
							cMap2D->SetMapInfo(y, 22, 24);
						}
					}
				}
			}
			ImGui::End();
			ImGui::PopStyleColor();

			//Materials
			{
				if (CurrentBarbWireLv == 0)
				{
					Wireitemlist[0] = "Scrap Metal";
					Wireitemlist[1] = "empty1";
					Wireitemlistcount[0] = 50;
					Wireitemlistcount[1] = 25;

				}
				if (CurrentBarbWireLv == 1)
				{
					Wireitemlist[0] = "empty1";
					Wireitemlist[1] = "empty2";
					Wireitemlistcount[0] = 50;
					Wireitemlistcount[1] = 225;
				}
				for (int i = 0; i < 2; i++)
				{
					if (Wireitemlist[i].find("empty") == string::npos)
					{
						ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.f, 0.3f, 1.0f));  // Set a background color
						ImGui::Begin("Wire Upgrade 2", NULL, window_flags_images);
						ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 3, CSettings::GetInstance()->iWindowHeight / 1.55));
						ImGui::SetWindowSize(ImVec2(400.0, 100.0));

						std::string itemlistcountstr = std::to_string(Wireitemlistcount[i]);
						cInventoryItem = cInventoryManager->GetItem(Wireitemlist[i]);
						ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(), ImVec2(70, 70), ImVec2(0, 1), ImVec2(1, 0));
						ImGui::SetWindowFontScale(1.0f * relativeScale_y);
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(1, 1, 0, 1), itemlistcountstr.c_str());
						ImGui::SameLine();

						ImGui::End();
						ImGui::PopStyleColor();
					}
				}

			}


		}
		


	ImGui::EndFrame();

	cScene2D->SetWindowUpgradeLvl(CurrentWindowLv);
	cScene2D->SetTurretUpgradeLvl(CurrentTurretLv);
	cScene2D->SetBarbwireUpgradeLvl(CurrentBarbWireLv);

	//For keyboard controls
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE) || CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_F10))
	{
		return true;
	}

}

/**
 @brief Render this class instance
 */
void CUpgradeState::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//cout << "CUpgradeState::Render()\n" << endl;
}

/**
 @brief Destroy this class instance
 */
void CUpgradeState::Destroy(void)
{
	// cout << "CUpgradeState::Destroy()\n" << endl;
}
