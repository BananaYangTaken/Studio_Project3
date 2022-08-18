#pragma once

// Include Parent Class Enemy Base
#include "EnemyBase.h"
#include "Player2D_V2.h"
class CEnemy2D_Zombie : public CEnemyBase
{
public:
	// Constructor
	CEnemy2D_Zombie(void);

	// Destructor
	~CEnemy2D_Zombie(void);
	int idlecount = 0;
	bool Init(int, int);
	void Update(const double dElapsedTime);
	bool checkforLOS();
	void UpdateToLastLOS();
	float DetectionRadius = 15;
	bool hasseenplayeronce = false;
	glm::vec2 Playervec2OldIndex;
	glm::vec2 OldPositu;

protected:
	CPlayer2D_V2* cPlayer;
	float deathTimer;
	float speedMultiplier;
	float motionResetter;
};

