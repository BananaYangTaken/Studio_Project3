#pragma once

// Include Parent Class Enemy Base
#include "EnemyBase.h"

class CEnemy2D_Skeleton : public CEnemyBase
{
public:
	// Constructor
	CEnemy2D_Skeleton(void);

	// Destructor
	~CEnemy2D_Skeleton(void);

	bool Init(int, int, std::vector<CProjectile2D*>* cProjectileList);

	virtual void Update(const double dElapsedTime);

protected:


	// Determine which player to target
	CPlayer2D_V2* DecideTarget(void);

};

