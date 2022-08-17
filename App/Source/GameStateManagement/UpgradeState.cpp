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

	CShaderManager::GetInstance()->Use("Shader2D");
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



	Barbwire[0].fileName = "Image\\GUI\\Barbwire.png";
	Barbwire[0].textureID = il->LoadTextureGetID(Barbwire[0].fileName.c_str(), false);

	Barbwire[1].fileName = "Image\\GUI\\BarbwireDone.png";
	Barbwire[1].textureID = il->LoadTextureGetID(Barbwire[1].fileName.c_str(), false);

	


	cSettings = CSettings::GetInstance();
	cSoundController = CSoundController::GetInstance();
	// Enable the cursor
	if (CSettings::GetInstance()->bDisableMousePointer == true)
		glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


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
bool CUpgradeState::Update(const double dElapsedTime)
{
	float buttonWidth = 250;
	float buttonHeight = 125;

	const float relativeScale_x = cSettings->iWindowWidth / 800.0f;
	const float relativeScale_y = cSettings->iWindowHeight / 600.0f;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		//window_flags |= ImGuiWindowFlags_MenuBar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoNav;
		window_flags |= ImGuiWindowFlags_NoResize;
		static float f = 0.0f;
		static int counter = 0;



		
		// Create a window called Main menu
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 1.0f));  // Set a background color
		ImGui::Begin("Main Menu", NULL, window_flags);
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

		//Window for Window Upgrade Button
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.f, 0.3f, 1.0f));  // Set a background color
		ImGui::Begin("Window Upgrade", NULL, window_flags);
		{
			ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 3, CSettings::GetInstance()->iWindowHeight / 3.2));				
			ImGui::SetWindowSize(ImVec2(640.0, 150.0));

			//window upgrade
			{
				ImGui::SetWindowFontScale(1.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "  Material cost:   ");
				ImGui::SameLine();
				if (WindowUpgradeLvl == 0)
				{
					if (ImGui::ImageButton((ImTextureID)WindowUpgrade[0].textureID,
						ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
					{
						WindowUpgradeLvl += 1;
						// Reset the CKeyboardController
						CKeyboardController::GetInstance()->Reset();
						CSoundController::GetInstance()->MasterVolumeIncrease(10);
					}
				}
				if (WindowUpgradeLvl == 1)
				{
					if (ImGui::ImageButton((ImTextureID)WindowUpgrade[1].textureID,
						ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
					{
						WindowUpgradeLvl += 1;
						// Reset the CKeyboardController
						CKeyboardController::GetInstance()->Reset();
						CSoundController::GetInstance()->MasterVolumeIncrease(10);
					}
				}
				if (WindowUpgradeLvl == 2)
				{
					if (ImGui::ImageButton((ImTextureID)WindowUpgrade[2].textureID,
						ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
					{
						// Reset the CKeyboardController
						CKeyboardController::GetInstance()->Reset();
						CSoundController::GetInstance()->MasterVolumeIncrease(10);
					}
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();

		//Window for Turret Upgrade Button
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.f, 0.3f, 1.0f));  // Set a background color
		ImGui::Begin("Turret Upgrade", NULL, window_flags);
		{
			ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 3, CSettings::GetInstance()->iWindowHeight / 2.2));				
			ImGui::SetWindowSize(ImVec2(640.0, 150.0));

			//Turret upgrade
			{
				ImGui::SetWindowFontScale(1.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "  Material cost:   ");
				ImGui::SameLine();
				if (TurretUpgradeLvl == 0)
				{
					if (ImGui::ImageButton((ImTextureID)TurretUpgrade[0].textureID,
						ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
					{
						TurretUpgradeLvl += 1;
						// Reset the CKeyboardController
						CKeyboardController::GetInstance()->Reset();

						CSoundController::GetInstance()->MasterVolumeIncrease(10);
					}
				}
				if (TurretUpgradeLvl == 1)
				{
					if (ImGui::ImageButton((ImTextureID)TurretUpgrade[1].textureID,
						ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
					{
						// Reset the CKeyboardController
						CKeyboardController::GetInstance()->Reset();

						CSoundController::GetInstance()->MasterVolumeIncrease(10);
					}
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();


		//Window for Barbwire Upgrade Button
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.f, 0.3f, 1.0f));  // Set a background color
		ImGui::Begin("Barbwire", NULL, window_flags);
		{
			ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 3, CSettings::GetInstance()->iWindowHeight /1.68));
			ImGui::SetWindowSize(ImVec2(640.0, 150.0));

			//Barbwire upgrade
			{
				ImGui::SetWindowFontScale(1.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "  Material cost:   ");
				ImGui::SameLine();
				if (BarbwireUpgrade == 0)
				{
					if (ImGui::ImageButton((ImTextureID)Barbwire[0].textureID,
						ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
					{
						BarbwireUpgrade += 1;
						// Reset the CKeyboardController
						CKeyboardController::GetInstance()->Reset();
						CSoundController::GetInstance()->MasterVolumeIncrease(10);
					}
				}
				if (BarbwireUpgrade == 1)
				{
					if (ImGui::ImageButton((ImTextureID)Barbwire[1].textureID,
						ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
					{
						// Reset the CKeyboardController
						CKeyboardController::GetInstance()->Reset();
						CSoundController::GetInstance()->MasterVolumeIncrease(10);
					}
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();
	
	ImGui::EndFrame();

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
