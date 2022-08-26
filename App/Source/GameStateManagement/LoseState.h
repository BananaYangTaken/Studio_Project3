#pragma once

/**
 CIntroState
 @brief This class is derived from CGameStateBase. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

#include "GameStateBase.h"
#include "../App/Source/Scene2D/GUI_Scene2D.h"

#include "Primitives/Mesh.h"
#include "../Scene2D/BackgroundEntity.h"
#include <string>

 // Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include CEntity2D
#include "Primitives/Entity2D.h"

 // FPS Counter
#include "TimeControl\FPSCounter.h"
#include "GameControl/Settings.h"

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


#include "../App/Source/Scene2D/Scene2D.h"

class CLoseState : public CGameStateBase
{
public:
	// Constructor
	CLoseState(void);
	// Destructor
	~CLoseState(void);

	// Init this class instance
	virtual bool Init(void);
	// Update this class instance
	virtual bool Update(const double dElapsedTime);
	// Render this class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);

	std::string DayNum;

protected:
	CBackgroundEntity* background;

	// FPS Control
	CFPSCounter* cFPSCounter;

	// Flags for IMGUI
	ImGuiWindowFlags window_flags;

	//Handler containing the instance of 2D Gui
	CGUI_Scene2D* cGUI_Scene2D;

	CScene2D* cScene2D;
};
