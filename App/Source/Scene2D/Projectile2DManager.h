#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"
#include "Projectile2D.h"
#include "Map2D.h"
#include <vector>
#include "GameControl/Settings.h"
#include "Physics2D.h"

class CProjectileManager : public CSingletonTemplate<CProjectileManager>
{
	friend CSingletonTemplate<CProjectileManager>;
public:
	void Exit(void);

	void Init(void);
	// Update
	void Update(const double dElapsedTime);
	
	//Render
	void PreRender(void);
	void Render(void);
	void PostRender(void);
	void Reset(void);
	void SpawnProjectile(glm::vec2 vec2Index, glm::vec2 vec2NumMicroSteps, glm::vec2 Direction, float Speed, CProjectile2D::TYPE Type, CProjectile2D::PROJECTILE Projectile, float Damage);
	void SetEnemyVector(std::vector<CEntity2D*>* cEnemyList);
	

protected:
	// Constructor
	CProjectileManager(void);
	// Destructor
	virtual ~CProjectileManager(void);

	CSettings* cSettings;
	CEntity2D* Player;
	std::vector<CEntity2D*>* cEnemyList;
	CMap2D* cMap2D;
	CPhysics2D cPhysics2D;

	//A vector containing instances of Projectile2D
	std::vector<CProjectile2D*> cProjectileList;

};
