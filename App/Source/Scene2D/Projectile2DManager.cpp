#include "Projectile2DManager.h"
#include <stdexcept>      // std::invalid_argument
#include "Player2D_V2.h"
#include "EnemyBase.h"

/**
@brief Constructor
*/
CProjectileManager::CProjectileManager(void)
	: cProjectileList(NULL)
	, cSettings(NULL)
	, cMap2D(NULL) 
	, cEnemyList(NULL) 
{
	Player = NULL;
}

/**
@brief Destructor
*/
CProjectileManager::~CProjectileManager(void)
{
	// Clear the memory
	Exit();
}


/**
@brief Exit by deleting the items
*/
void CProjectileManager::Exit(void)
{
	cProjectileList.clear();
}

void CProjectileManager::Init(void)
{
	Player = dynamic_cast<CEntity2D*>(CPlayer2D_V2::GetInstance());
	cMap2D = CMap2D::GetInstance();
	cSettings = CSettings::GetInstance();
	cPhysics2D.Init();
}

void CProjectileManager::Update(const double dElapsedTime)
{
	//std::cout << cProjectileList.size() << std::endl;
	//cout << cPhysics2D.CalculateDistance(Player->vec2Index, (*it)->vec2Index, 'n', true, Player->vec2NumMicroSteps, (*it)->vec2NumMicroSteps);
							
	//Collision Detection
	if (cProjectileList.size() > 0)
	{
		vector<CProjectile2D*>::iterator it = cProjectileList.begin();
		bool CollisionDetected = false;
		while (it != cProjectileList.end())
		{
			if (*it && !CollisionDetected)
			{
				//Projectile Collision with Player
				if ((*it)->GetType() == CProjectile2D::TYPE::HOSTILE || (*it)->GetType() == CProjectile2D::TYPE::NEUTRAL)
				{
				
					//There is collision
					if (cPhysics2D.CalculateDistance(Player->vec2Index, (*it)->vec2Index, 'n', true, Player->vec2NumMicroSteps, (*it)->vec2NumMicroSteps) < 10)
					{
						if (dynamic_cast<CPlayer2D_V2*>(Player)->GetInvulnerabilityFrame() >= 0)
						{
							dynamic_cast<CPlayer2D_V2*>(Player)->SetHealth(dynamic_cast<CPlayer2D_V2*>(Player)->GetHealth() - (*it)->GetDamage());
							dynamic_cast<CPlayer2D_V2*>(Player)->SetInvulnerabilityFrame(0.3);
						
						}
						CollisionDetected = !CollisionDetected;
						break;
					}
				}

				//Projectile Collision with Enemy
				if ((*it)->GetType() == CProjectile2D::FRIENDLY
					|| (*it)->GetType() == CProjectile2D::NEUTRAL)
				{
					for (int i = 0; i < cEnemyList->size(); ++i)
					{
						//There is collision
						if (cPhysics2D.CalculateDistance(cEnemyList->at(i)->vec2Index, (*it)->vec2Index, 'n', true, cEnemyList->at(i)->vec2NumMicroSteps, (*it)->vec2NumMicroSteps) < 24)
						{
							if (dynamic_cast<CEnemyBase*>(cEnemyList->at(i))->GetInvulnerabilityFrame() <= 0)
							{
								dynamic_cast<CEnemyBase*>(cEnemyList->at(i))->SetHealth(dynamic_cast<CEnemyBase*>(cEnemyList->at(i))->GetHealth() - (*it)->GetDamage());
								dynamic_cast<CEnemyBase*>(cEnemyList->at(i))->SetInvulnerabilityFrame(0.2);
								dynamic_cast<CEnemyBase*>(cEnemyList->at(i))->SetStun(true);
							}
							CollisionDetected = !CollisionDetected;
							break;
						}
					}
				}

				//Projectile Collision with Border
				if((*it)->vec2Index.x < 0 || (*it)->vec2Index.x > cSettings->NUM_TILES_XAXIS-1 || (*it)->vec2Index.y < 0 || (*it)->vec2Index.y > cSettings->NUM_TILES_YAXIS-1)
				{
					CollisionDetected = !CollisionDetected;
				}
				else if (cMap2D->GetMapInfo((*it)->vec2Index.y, (*it)->vec2Index.x) >= 100) //Projectile Collision with Map
				{
					CollisionDetected = !CollisionDetected;
				}
			}

			if (CollisionDetected)
			{
				CollisionDetected = !CollisionDetected;
				it = cProjectileList.erase(it);
			}
			else if ((*it)->GetExistenceTimer() < 0 && !CollisionDetected)
			{
				//delete projectiles that have existential timers
				it = cProjectileList.erase(it);
			}
			else
			{
				++it;
			}
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

void CProjectileManager::SpawnProjectile(glm::vec2 vec2Index, glm::vec2 vec2NumMicroSteps, float Rotation, float Speed, CProjectile2D::TYPE Type, CProjectile2D::PROJECTILE Projectile, float Damage)
{
	CProjectile2D* Temp = new CProjectile2D;
	Temp->Init(vec2Index, vec2NumMicroSteps, Rotation, Speed, Type, Projectile, Damage);
	cProjectileList.push_back(Temp);
}

void CProjectileManager::SetEnemyVector(std::vector<CEntity2D*>* cEnemyList)
{
	this->cEnemyList = cEnemyList;
}

