/**
 CPlayer2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"


class CObject2D : public CEntity2D
{
public:

	// Init
	bool Init(int, int);

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
	CObject2D(void);

	// Constructor
	CObject2D(int);

	// Destructor
	virtual ~CObject2D(void);

	//Get Handlers
	CMap2D* GetMap2D(void);
	CSettings* GetSettings(void);

	void SetTransition(bool);
	void SetIdle(bool);
	bool GetIdle(void);
	void SetPlayervec2Index(glm::vec2);
	void Interact(void);

	void SetruntimeColour(glm::vec4);

protected:
	// Handler to the CMap2D instance
	CMap2D* cMap2D;
	bool InteractedWith;

	//Object Type
	int ObjectID;
	bool Transition;
	bool Idle;

	//Object Animation
	CSpriteAnimation* animatedSprites;

	glm::vec2 Playervec2Index;

	// Object's colour
	glm::vec4 runtimeColour;
};