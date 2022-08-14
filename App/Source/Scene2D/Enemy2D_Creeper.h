#pragma once

// Include Parent Class Enemy Base
#include "EnemyBase.h"

class CEnemy2D_Creeper : public CEnemyBase
{
public:
	// Constructor
	CEnemy2D_Creeper(void);

	// Destructor
	~CEnemy2D_Creeper(void);

	bool Init(int, int, std::vector<CProjectile2D*>*);

	void Update(const double dElapsedTime);

protected:
	// Determine which player to target
	CPlayer2D_V2* DecideTarget(void);  

	float ExplosionDuration;
};

