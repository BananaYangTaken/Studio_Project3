/**
 CInventoryManager
 @brief A class which manages all the inventory items the game
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"

#include "EnemyBase.h"
#include "Player2D_V2.h"


class CEntityManager : public CSingletonTemplate<CEntityManager>
{
	friend CSingletonTemplate<CEntityManager>;
public:

	//Getters and Setters for Entities
	void SetPlayer(CPlayer2D_V2* Player);
	CPlayer2D_V2* GetPlayer(void);

	void SetEnemy(CEnemyBase* newEnemyList);

protected:
	// Constructor
	CEntityManager(void);

	// Destructor
	virtual ~CEntityManager(void);

	CPlayer2D_V2* Player;
	std::vector<CEnemyBase*>* cEnemyList;

};


class CPlayerData : public CSingletonTemplate<CPlayerData>
{
	friend CSingletonTemplate<CPlayerData>;
public:

	//Getters and Setters for Player Data

protected:
	// Constructor
	CPlayerData(void);

	// Destructor
	virtual ~CPlayerData(void);



};
