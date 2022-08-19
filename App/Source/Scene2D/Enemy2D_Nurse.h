#pragma once

// Include Parent Class Enemy Base
#include "EnemyBase.h"
#include "Player2D_V2.h"
class CEnemy2D_Nurse : public CEnemyBase
{
public:
	// Constructor
	CEnemy2D_Nurse(void);

	// Destructor
	~CEnemy2D_Nurse(void);
	int idlecount = 0;
	bool Init(int, int);
	void Update(const double dElapsedTime);
	bool checkforLOS();
	void UpdateToLastLOS();
	float DetectionRadius = 20;
	bool hasseenplayeronce = false;
	glm::vec2 Playervec2OldIndex;
	glm::vec2 OldPositu;
	void droploot();
	void spawnloot( float vecX, float vecY);
protected:
	float deathTimer;
	float speedMultiplier;
	float motionResetter;
	float healTimer;
	int prevHP;
	float readyToHeal;
	float redTimer;
};

