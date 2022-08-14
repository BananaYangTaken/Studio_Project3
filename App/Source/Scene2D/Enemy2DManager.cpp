#include "Enemy2DManager.h"
#include <stdexcept>      // std::invalid_argument

/**
@brief Constructor
*/
CEnemyManager::CEnemyManager(void)
	: cEnemyList(NULL)
	, cSettings(NULL)
	, cMap2D(NULL) 
{
}

/**
@brief Destructor
*/
CEnemyManager::~CEnemyManager(void)
{
	// Clear the memory
	Exit();
}

/**
@brief Exit by deleting the items
*/
void CEnemyManager::Exit(void)
{
	cEnemyList.clear();
}

void CEnemyManager::Init()
{
	cMap2D = CMap2D::GetInstance();
	cSettings = CSettings::GetInstance();
}

void CEnemyManager::Update(const double dElapsedTime)
{
	vector<CEnemyBase*>::iterator it = cEnemyList.begin();
	while (it != cEnemyList.end())
	{
		if ((*it) && (*it)->GetHealth() <= 0)
		{
			it = cEnemyList.erase(it);
		}
		else
		{
			++it;
		}
	}
	//Update Movement
	for (int i = 0; i < cEnemyList.size(); i++)
	{
		cEnemyList.at(i)->Update(dElapsedTime);
	}
}

void CEnemyManager::PreRender(void)
{
	for (int i = 0; i < cEnemyList.size(); i++)
	{
		cEnemyList.at(i)->PreRender();
	}
}

void CEnemyManager::Render(void)
{
	for (int i = 0; i < cEnemyList.size(); i++)
	{
		cEnemyList.at(i)->Render();
	}
}

void CEnemyManager::PostRender(void)
{
	for (int i = 0; i < cEnemyList.size(); i++)
	{
		cEnemyList.at(i)->PostRender();
	}
}

void CEnemyManager::Reset(void)
{
	cEnemyList.clear();
}

std::vector<CEnemyBase*>* CEnemyManager::GetEnemyList(void)
{
	return &cEnemyList;
}


