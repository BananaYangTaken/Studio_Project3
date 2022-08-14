#include "Projectile2DManager.h"
#include <stdexcept>      // std::invalid_argument
#include "Enemy2D_Human.h"
#include "Enemy2D_Creeper.h"

/**
@brief Constructor
*/
CProjectileManager::CProjectileManager(void)
	: cProjectileList(NULL)
	, cSettings(NULL)
	, cMap2D(NULL) 
	, cEnemyList(NULL) 
{
	Players[0] = NULL;
	Players[1] = NULL;
}

/**
@brief Destructor
*/
CProjectileManager::~CProjectileManager(void)
{
	// Clear the memory
	Exit();
}

float CProjectileManager::CalcDistance(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos1Steps, glm::vec2 pos2Steps, char axis = 'x')
{
	float MicroStepDist = 0;
	if (axis == 'x') 
	{
		if (pos1.x - 1 > 0)
		{
			MicroStepDist += (pos1.x - 1) * cSettings->NUM_STEPS_PER_TILE_XAXIS;
		}
		if (pos2.x - 1 > 0)
		{
			MicroStepDist -= (pos2.x - 1) * cSettings->NUM_STEPS_PER_TILE_XAXIS;
		}
		MicroStepDist += pos1Steps.x - pos2Steps.x;
		MicroStepDist = abs(MicroStepDist);
	}
	else if (axis == 'y')
	{
		if (pos1.y - 1 > 0)
		{
			MicroStepDist += (pos1.y - 1) * cSettings->NUM_STEPS_PER_TILE_YAXIS;
		}
		if (pos2.y - 1 > 0)
		{
			MicroStepDist -= (pos2.y - 1) * cSettings->NUM_STEPS_PER_TILE_YAXIS;
		}
		MicroStepDist = pos1Steps.y - pos2Steps.y;
		MicroStepDist = abs(MicroStepDist);
	}
	return MicroStepDist;

}

/**
@brief Exit by deleting the items
*/
void CProjectileManager::Exit(void)
{
	cProjectileList.clear();
}

void CProjectileManager::Init(CPlayer2D_V2* Player1, CPlayer2D_V2* Player2)
{
	Players[0] = Player1;
	Players[1] = Player2;
	cMap2D = CMap2D::GetInstance();
	cSettings = CSettings::GetInstance();
}

void CProjectileManager::Update(const double dElapsedTime)
{
	//Collision Detection
	vector<CProjectile2D*>::iterator it = cProjectileList.begin();
	bool CollisionDetected = false;
	while (it != cProjectileList.end())
	{
		//Projectile Collision with Player
		if (*it && !CollisionDetected)
		{
			for (int i = 0; i < 2; ++i)
			{
				if ((*it)->GetType() == CProjectile2D::HOSTILE || (*it)->GetType() == CProjectile2D::NEUTRAL)
				{
					//There is collision
					if ((Players[i]->vec2Index.y == (*it)->vec2Index.y)
						&& CalcDistance(Players[i]->vec2Index, (*it)->vec2Index, Players[i]->vec2NumMicroSteps, (*it)->vec2NumMicroSteps, 'x') < 10
						&& CalcDistance(Players[i]->vec2Index, (*it)->vec2Index, Players[i]->vec2NumMicroSteps, (*it)->vec2NumMicroSteps, 'y') < 1)
					{
						if (Players[i]->GetInvulnerabilityFrame() >= 0)
						{
							Players[i]->SetHealth(Players[i]->GetHealth() - 1);
							Players[i]->SetInvulnerabilityFrame(0.5);
							if (i == 0)
							{
								CSoundController::GetInstance()->PlaySoundByID(5);
							}
							else
							{
								CSoundController::GetInstance()->PlaySoundByID(4);
							}
						}
						CollisionDetected = !CollisionDetected;
						break;
					}
				}
			}

			//Projectile Collision with Enemy
			for (int i = 0; i < cEnemyList->size(); ++i)
			{
				if ((*it)->GetType() == CProjectile2D::FRIENDLY
					|| (*it)->GetType() == CProjectile2D::NEUTRAL)
				{
					//There is collision
					if (cEnemyList->at(i)->vec2Index.y == (*it)->vec2Index.y
						&& CalcDistance(cEnemyList->at(i)->vec2Index, (*it)->vec2Index, cEnemyList->at(i)->vec2NumMicroSteps, (*it)->vec2NumMicroSteps, 'x') < 10
						&& CalcDistance(cEnemyList->at(i)->vec2Index, (*it)->vec2Index, cEnemyList->at(i)->vec2NumMicroSteps, (*it)->vec2NumMicroSteps, 'y') < 1)
					{
						if (cEnemyList->at(i)->GetInvulnerabilityFrame() <= 0)
						{
							cEnemyList->at(i)->SetHealth(cEnemyList->at(i)->GetHealth() - 1);
							cEnemyList->at(i)->SetInvulnerabilityFrame(0.5);
							cEnemyList->at(i)->SetStun(true);
							CSoundController::GetInstance()->PlaySoundByID(4);
						}
						CollisionDetected = !CollisionDetected;
						break;
					}
				}
			}

			//Projectile Collision with Map
			if (cMap2D->GetMapInfo((*it)->vec2Index.y, (*it)->vec2Index.x) >= 100)
			{
				CollisionDetected = !CollisionDetected;
			}
		}
		
		if (CollisionDetected)
		{
			CollisionDetected = !CollisionDetected;
			it = cProjectileList.erase(it);
			CSoundController::GetInstance()->PlaySoundByID(6);
		}
		else
		{
			++it;
		}
	}
	//Update Movement
	for (int i = 0; i < cProjectileList.size(); i++)
	{
		cProjectileList.at(i)->Update(dElapsedTime);
	}
}

void CProjectileManager::PreRender(void)
{
	for (int i = 0; i < cProjectileList.size(); i++)
	{
		cProjectileList.at(i)->PreRender();
	}
}

void CProjectileManager::Render(void)
{
	for (int i = 0; i < cProjectileList.size(); i++)
	{
		cProjectileList.at(i)->Render();
	}
}

void CProjectileManager::PostRender(void)
{
	for (int i = 0; i < cProjectileList.size(); i++)
	{
		cProjectileList.at(i)->PostRender();
	}
}

void CProjectileManager::Reset(void)
{
	cProjectileList.clear();
}

std::vector<CProjectile2D*>* CProjectileManager::GetProjectileList(void)
{
	return &cProjectileList;
}

void CProjectileManager::SetEnemyVector(std::vector<CEnemyBase*>* cEnemyList)
{
	this->cEnemyList = cEnemyList;
}

