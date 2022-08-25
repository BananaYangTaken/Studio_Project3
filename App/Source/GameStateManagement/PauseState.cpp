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

#include "PauseState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include Mesh Builder
#include "Primitives/MeshBuilder.h"
// Include ImageLoader
#include "System\ImageLoader.h"
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
CPauseState::CPauseState(void)
	//: background(NULL)
{

}

/**
 @brief Destructor
 */
CPauseState::~CPauseState(void)
{

}

/**
 @brief Init this class instance
 */
bool CPauseState::Init(void)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	cout << "CPauseState::Init()\n" << endl;

	CShaderManager::GetInstance()->Use("Shader2D");
	//CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	// Load the images for buttons
	CImageLoader* il = CImageLoader::GetInstance();
	for (int i = 0; i < 5; ++i)
	{
		VolumeIncrease1ButtonData[i].fileName = "Image\\GUI\\Plus1.png";
		VolumeIncrease1ButtonData[i].textureID = il->LoadTextureGetID(VolumeIncrease1ButtonData[i].fileName.c_str(), false);
		VolumeDecrease1ButtonData[i].fileName = "Image\\GUI\\Minus1.png";						
		VolumeDecrease1ButtonData[i].textureID = il->LoadTextureGetID(VolumeDecrease1ButtonData[i].fileName.c_str(), false);
		VolumeIncrease10ButtonData[i].fileName = "Image\\GUI\\Plus10.png";
		VolumeIncrease10ButtonData[i].textureID = il->LoadTextureGetID(VolumeIncrease10ButtonData[i].fileName.c_str(), false);
		VolumeDecrease10ButtonData[i].fileName = "Image\\GUI\\Minus10.png";
		VolumeDecrease10ButtonData[i].textureID = il->LoadTextureGetID(VolumeDecrease10ButtonData[i].fileName.c_str(), false);
	}

	cSettings = CSettings::GetInstance();
	cSoundController = CSoundController::GetInstance();
	// Enable the cursor
	if (CSettings::GetInstance()->bDisableMousePointer == true)
		glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	return true;
}

/**
 @brief Update this class instance
 */
bool CPauseState::Update(const double dElapsedTime)
{
	cSettings->bShowMousePointer = true;
	float buttonWidth = 32;
	float buttonHeight = 32;

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
		static float f = 0.0f;
		static int counter = 0;

		
		// Create a window called "Hello, world!" and append into it.
		ImGui::Begin("Main Menu", NULL, window_flags);
		{
			ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 4.0,
				CSettings::GetInstance()->iWindowHeight / 4.0));				// Set the top-left of the window at (10,10)
			ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth/2, CSettings::GetInstance()->iWindowHeight/2));

			//Added rounding for nicer effect
			ImGuiStyle& style = ImGui::GetStyle();
			style.FrameRounding = 200.0f;

			//MasterVolume
			{
				ImGui::TextColored(ImVec4(1, 1, 1, 1), "In-Game Menu");
				ImGui::SetWindowFontScale(1.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Master Volume:");
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeIncrease10ButtonData[0].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->MasterVolumeIncrease(10);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeIncrease1ButtonData[0].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->MasterVolumeIncrease();
				}
				ImGui::SameLine();
				int MasterVolume = 100 * (cSoundController->GetMasterVolume());
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d / 100", MasterVolume);
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeDecrease1ButtonData[0].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->MasterVolumeDecrease();
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeDecrease10ButtonData[0].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->MasterVolumeDecrease(10);
				}

			}
			//Player Volume
			{
				ImGui::SetWindowFontScale(1.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Player Volume:");
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeIncrease10ButtonData[1].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeIncrease(CSoundInfo::SOUNDGROUP::Player, 10);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeIncrease1ButtonData[1].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeIncrease(CSoundInfo::SOUNDGROUP::Player);
				}
				ImGui::SameLine();
				int SoundGroupVolume = 100 * (cSoundController->GetSoundGroupVolume(CSoundInfo::SOUNDGROUP::Player));
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d / 100", SoundGroupVolume);
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeDecrease1ButtonData[1].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeDecrease(CSoundInfo::SOUNDGROUP::Player);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeDecrease10ButtonData[1].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeDecrease(CSoundInfo::SOUNDGROUP::Player, 10);
				}

			}
			//Creatures Volume
			{
				ImGui::SetWindowFontScale(1.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Creatures Volume:");
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeIncrease10ButtonData[2].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeIncrease(CSoundInfo::SOUNDGROUP::Creatures, 10);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeIncrease1ButtonData[2].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeIncrease(CSoundInfo::SOUNDGROUP::Creatures);
				}
				ImGui::SameLine();
				int SoundGroupVolume = 100 * (cSoundController->GetSoundGroupVolume(CSoundInfo::SOUNDGROUP::Creatures));
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d / 100", SoundGroupVolume);
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeDecrease1ButtonData[2].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeDecrease(CSoundInfo::SOUNDGROUP::Creatures);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeDecrease10ButtonData[2].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeDecrease(CSoundInfo::SOUNDGROUP::Creatures, 10);
				}

			}
			//Environment Volume
			{
				ImGui::SetWindowFontScale(1.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Environment Volume:");
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeIncrease10ButtonData[3].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeIncrease(CSoundInfo::SOUNDGROUP::Environment, 10);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeIncrease1ButtonData[3].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeIncrease(CSoundInfo::SOUNDGROUP::Environment);
				}
				ImGui::SameLine();
				int SoundGroupVolume = 100 * (cSoundController->GetSoundGroupVolume(CSoundInfo::SOUNDGROUP::Environment));
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d / 100", SoundGroupVolume);
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeDecrease1ButtonData[3].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeDecrease(CSoundInfo::SOUNDGROUP::Environment);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeDecrease10ButtonData[3].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeDecrease(CSoundInfo::SOUNDGROUP::Environment, 10);
				}

			}
			//Background Volume
			{
				ImGui::SetWindowFontScale(1.5f * relativeScale_y);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Background Volume:");
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeIncrease10ButtonData[4].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeIncrease(CSoundInfo::SOUNDGROUP::Background, 10);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeIncrease1ButtonData[4].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeIncrease(CSoundInfo::SOUNDGROUP::Background);
				}
				ImGui::SameLine();
				int SoundGroupVolume = 100 * (cSoundController->GetSoundGroupVolume(CSoundInfo::SOUNDGROUP::Background));
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d / 100", SoundGroupVolume);
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeDecrease1ButtonData[4].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeDecrease(CSoundInfo::SOUNDGROUP::Background);
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((ImTextureID)VolumeDecrease10ButtonData[4].textureID,
					ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
				{
					// Reset the CKeyboardController
					CKeyboardController::GetInstance()->Reset();

					CSoundController::GetInstance()->SoundGroupVolumeDecrease(CSoundInfo::SOUNDGROUP::Background, 10);
				}

			}
			
		}
		ImGui::End();
		
		/*
		// Create an invisible window which covers the entire OpenGL window
		ImGui::Begin("Invisible window", NULL, window_flags);
		{
			ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.0 - buttonWidth / 2.0,
				CSettings::GetInstance()->iWindowHeight / 3.0));				// Set the top-left of the window at (10,10)
			ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));


			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f,0.0f,0.2f, 1.0f));  // Set a background color

			ImGuiWindowFlags window_flags1 = 0;
			window_flags1 |= ImGuiWindowFlags_NoTitleBar;
			window_flags1 |= ImGuiWindowFlags_NoMove;
			window_flags1 |= ImGuiWindowFlags_NoCollapse;
			window_flags1 |= ImGuiWindowFlags_NoNav;
			{
				ImGui::Begin("Scrollbar", NULL, window_flags1);
				{
					ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.0 - buttonWidth / 2.0,
						CSettings::GetInstance()->iWindowHeight / 3.0));				// Set the top-left of the window at (10,10)
					ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));
					ImGui::SameLine();
					ImGui::SetWindowFontScale(1.5f * relativeScale_y);
					ImGui::TextColored(ImVec4(1, 1, 0, 1), "Master Volume %d", cSoundController->GetMasterVolume());
				}
				ImGui::End();
			}
			ImGui::PopStyleColor();
		}
		ImGui::End();
		*/
			
	
	ImGui::EndFrame();

	//For keyboard controls
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE) || CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_F10))
	{
		/*if (CGameStateManager::GetInstance()->GetPrevGameState() == CGameStateBase* )
		{

		}*/

		return true;
	}

}

/**
 @brief Render this class instance
 */
void CPauseState::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//cout << "CPauseState::Render()\n" << endl;
}

/**
 @brief Destroy this class instance
 */
void CPauseState::Destroy(void)
{
	// cout << "CPauseState::Destroy()\n" << endl;
}
