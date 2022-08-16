#pragma once

// Include Parent Class Enemy Base
#include "EnemyBase.h"

class CEnemy2D_Zombie : public CEnemyBase
{
public:
	// Constructor
	CEnemy2D_Zombie(void);

	// Destructor
	~CEnemy2D_Zombie(void);

	bool Init(int, int);

	void Update(const double dElapsedTime);

protected:

	unsigned int HealCount;
	float HealDuration;
	float deathTimer;
};

