#pragma once

// Include Parent Class Enemy Base
#include "EnemyBase.h"

class CEnemy2D_V2 : public CEnemyBase
{
public:
	// Constructor
	CEnemy2D_V2(void);

	// Destructor
	~CEnemy2D_V2(void);

	bool Init(int, int, std::vector<CProjectile2D*>*);

};

