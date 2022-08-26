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

#include "LoseState.h"

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

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CLoseState::CLoseState(void)
	: background(NULL)
	, cGUI_Scene2D(NULL)
	, cScene2D(NULL)
{

}

/**
 @brief Destructor
 */
CLoseState::~CLoseState(void)
{
	cGUI_Scene2D->Destroy();
	cScene2D->Destroy();
}

/**
 @brief Init this class instance
 */
bool CLoseState::Init(void)
{
	//cout << "CIntroState::Init()\n" << endl;

	// Include Shader Manager
	CShaderManager::GetInstance()->Use("Shader2D");
	//CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
	
	//Create and initialse 2D GUI
	cGUI_Scene2D = CGUI_Scene2D::GetInstance();
	cScene2D = CScene2D::GetInstance();
	// Store the CFPSCounter singleton instance here
	cFPSCounter = CFPSCounter::GetInstance();
	//Create Background Entity
	background = new CBackgroundEntity("Image/Lose.png");
	background->SetShader("Shader2D");
	background->Init();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(CSettings::GetInstance()->pWindow, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	DayNum = "...";

	return true;
}

/**
 @brief Update this class instance
 */
bool CLoseState::Update(const double dElapsedTime)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoNav;
		window_flags |= ImGuiWindowFlags_NoMouseInputs;
		window_flags |= ImGuiWindowFlags_NoResize;

		std::string temp = "Days Survived : ";
		DayNum = temp + std::to_string(cScene2D->days);
		// display days
		ImGui::Begin("DaysCount", NULL, window_flags);
		{
			ImVec4 col = ImVec4(0.f, 1.f, 0.f, 1.f);
			ImGui::PushStyleColor(ImGuiCol_Text, col);
			ImGui::SetWindowPos(ImVec2(800, 600));
			ImGui::SetWindowSize(ImVec2(300, 50));
			ImGui::SetWindowFontScale(1.5f);
			ImGui::TextColored(ImVec4(1, 1, 0, 1), DayNum.c_str(), cFPSCounter->GetFrameRate());
			ImGui::PopStyleColor();
		}
	}
	ImGui::End();
	ImGui::EndFrame();



	//cout << "CIntroState::Update()\n" << endl;
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_SPACE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		//cout << "Loading MenuState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		return true;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CLoseState::Render()
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Draw the background
 	background->Render();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 @brief Destroy this class instance
 */
void CLoseState::Destroy(void)
{
	// Delete the background
	/*if (background)
	{
		delete background;
		background = NULL;
	}*/

	//cout << "CIntroState::Destroy()\n" << endl;
}