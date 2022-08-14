#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"
#include "Projectile2D.h"
#include "Player2D_V2.h"
#include "Map2D.h"
#include "EnemyBase.h"
#include <vector>
#include "GameControl/Settings.h"

class CEnemyManager : public CSingletonTemplate<CEnemyManager>
{
	friend CSingletonTemplate<CEnemyManager>;
public:
	void Exit(void);

	void Init();
	// Update
	void Update(const double dElapsedTime);
	
	//Render
	void PreRender(void);
	void Render(void);
	void PostRender(void);
	void Reset(void);

	std::vector<CEnemyBase*>* GetEnemyList(void);
	

protected:
	// Constructor
	CEnemyManager(void);
	// Destructor
	virtual ~CEnemyManager(void);


	CSettings* cSettings;
	CMap2D* cMap2D;
	std::vector<CEnemyBase*> cEnemyList;
};
