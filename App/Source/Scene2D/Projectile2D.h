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

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include SoundController
#include "..\SoundController\SoundController.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"
#include "Physics2D.h"

class CProjectile2D : public CEntity2D
{
public:
	enum TYPE
	{
		HOSTILE = 0,
		NEUTRAL = 1,
		FRIENDLY = 2,
		NUM_TYPE
	};
	enum PROJECTILE
	{
		BULLET = 0,
		KNIFE = 1,
		NUM_PROJECTILE
	};

	// Init
	bool Init(glm::vec2 vec2Index, glm::vec2 vec2NumMicroSteps, float Rotation, float Speed, TYPE Type, PROJECTILE Projectile, float Damage);

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
	CProjectile2D(void);

	// Destructor
	virtual ~CProjectile2D(void);

	TYPE GetType(void);
	float GetDamage(void);
	float GetExistenceTimer(void);

protected:
	CPhysics2D cPhysics2D;

	glm::vec2 vec2OldIndex;
	glm::vec2 Direction;
	CSpriteAnimation* animatedSprites;
	CEntity2D* Player;
	CSoundController* cSoundController;

	glm::vec4 runtimeColour;
	float Speed; //tile per second
	TYPE Type;
	PROJECTILE Projectile;
	float Damage;

	float ExistenceTimer;
};

