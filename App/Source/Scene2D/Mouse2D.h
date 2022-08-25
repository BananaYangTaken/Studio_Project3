#pragma once

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif
#include "GUI_Scene2D.h";
// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include SoundController
#include "..\SoundController\SoundController.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"
#include "Physics2D.h"

class CMouse2D : public CEntity2D
{
public:

	// Init
	bool Init();

	// Reset
	bool Reset(void);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	// Constructor
	CMouse2D(void);

	// Destructor
	virtual ~CMouse2D(void);

protected:
	CPhysics2D cPhysics2D;
	glm::vec4 runtimeColour;
	CSpriteAnimation* animatedSprites;
};

