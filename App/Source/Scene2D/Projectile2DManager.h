#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"
#include "Projectile2D.h"
#include "Player2D_V2.h"
#include "Map2D.h"
#include "EnemyBase.h"
#include <vector>
#include "GameControl/Settings.h"

class CProjectileManager : public CSingletonTemplate<CProjectileManager>
{
	friend CSingletonTemplate<CProjectileManager>;
public:
	void Exit(void);

	void Init(CPlayer2D_V2*, CPlayer2D_V2*);
	// Update
	void Update(const double dElapsedTime);
	
	//Render
	void PreRender(void);
	void Render(void);
	void PostRender(void);
	void Reset(void);

	std::vector<CProjectile2D*>* GetProjectileList(void);
	void SetEnemyVector(std::vector<CEnemyBase*>* cEnemyList);
	

protected:
	// Constructor
	CProjectileManager(void);
	// Destructor
	virtual ~CProjectileManager(void);

	float CalcDistance(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos1Steps, glm::vec2 pos2Steps, char axis);

	CSettings* cSettings;
	CPlayer2D_V2* Players[2];
	CMap2D* cMap2D;
	std::vector<CEnemyBase*>* cEnemyList;


	//A vector containing instances of Projectile2D
	std::vector<CProjectile2D*> cProjectileList;

};
