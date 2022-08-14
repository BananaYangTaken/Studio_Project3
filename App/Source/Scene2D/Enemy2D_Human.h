#pragma once

// Include Parent Class Enemy Base
#include "EnemyBase.h"

class CEnemy2D_Human : public CEnemyBase
{
public:
	// Constructor
	CEnemy2D_Human(void);

	// Destructor
	~CEnemy2D_Human(void);

	bool Init(int, int, std::vector<CProjectile2D*>*);

	void Update(const double dElapsedTime);

protected:

	unsigned int HealCount;
	float HealDuration;
};

