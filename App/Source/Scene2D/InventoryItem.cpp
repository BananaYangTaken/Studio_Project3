/**
 CInventoryItem
 @brief A class which represents an inventory item which can be collected in the game
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "InventoryItem.h"

// Include ImageLoader
#include "System\ImageLoader.h"

#include <iostream>

/**
@brief Constructor
*/
CInventoryItem::CInventoryItem(const char* imagePath)
	: iItemCount(0)
	, iItemMaxCount(0)
	, vec2Size(glm::vec2(0.0f))
	, itemtype(ITEM_TYPE::RESOURCES)
	, WData(NULL)
	, GData(NULL)
{
	if (imagePath)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID(imagePath, true);
		if (iTextureID == 0)
		{
			cout << "Unable to load " << imagePath << endl;
		}
	}
}

/**
@brief Destructor
*/
CInventoryItem::~CInventoryItem(void)
{
	/*
	if (WData)
	{
		delete WData;
		WData = NULL;
	}
	if (GData)
	{
		delete GData;
		GData = NULL;
	}*/
}

/**
@brief Add a number of items from this class instance
*/
void CInventoryItem::Add(const int iNumItems)
{
	iItemCount += iNumItems;
	if (iItemCount > iItemMaxCount)
		iItemCount = iItemMaxCount;
}

/**
@brief Remove the number of items from this class instance
*/
void CInventoryItem::Remove(const int iNumItems)
{
	iItemCount -= iNumItems;
	if (iItemCount < 0)
		iItemCount = 0;
}

/**
@brief Get the number of items from this class instance
*/
int CInventoryItem::GetCount(void) const
{
	return iItemCount;
}

/**
@brief  Get the maximum number of items from this class instance
*/
int CInventoryItem::GetMaxCount(void) const
{
	return iItemMaxCount;
}

void CInventoryItem::Load(void)
{
	//Loads Item Type
	if (sName == "Rifle" || sName == "Pistol")
	{
		itemtype = ITEM_TYPE::GUN;
		GData = new GunData;
		WData = new WeaponData;
		if (sName == "Pistol")
		{
			GData->firingtype = SEMIAUTO;
			GData->MaxAmmoSize = 17;
			WData->Damage = 20;
			GData->FireRate = 0.3;
			WData->ReloadTime = 0.5f;
		}
		else if (sName == "Rifle")
		{
			GData->firingtype = FULLAUTO;
			GData->MaxAmmoSize = 30;
			WData->Damage = 30;
			GData->FireRate = 0.1;
			WData->ReloadTime = 1;
		}
		GData->CurrentAmmoSize = GData->MaxAmmoSize;
		WData->ReloadTimer = 0;
		GData->FiringCooldown = 0;
	}
	else
	{
		GData = NULL;
		WData = NULL;
	}
}

/**
@brief Get the texture ID
*/
unsigned int CInventoryItem::GetTextureID(void) const
{
	return iTextureID;
}