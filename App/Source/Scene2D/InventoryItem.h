/**
 CInventoryItem
 @brief A class which represents an inventory item which can be collected in the game
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include CEntity2D
#include "Primitives/Entity2D.h"

#include <map>
#include <string>

enum FIRING_TYPE
{
	FULLAUTO = 0,
	SEMIAUTO = 1,
	NUM_FIRING_TYPE
};

enum ITEM_TYPE
{
	RESOURCES = 0,
	GUN = 1,
	MELEE = 2,
	CONSUMABLES = 3,
	NUM_ITEM_TYPE
};

struct GunData
{
	unsigned int MaxAmmoSize;
	unsigned int CurrentAmmoSize;
	FIRING_TYPE firingtype;
};
struct WeaponData
{
	float Damage;
	float ReloadTime;
};

class CInventoryItem : public CEntity2D
{
public:
	// Constructor
	CInventoryItem(const char* imagePath = NULL);

	// Destructor
	virtual ~CInventoryItem(void);

	// Add a number of items from this class instance
	void Add(const int iNumItems);
	// Remove the number of items from this class instance
	void Remove(const int iNumItems);
	// Get the number of items from this class instance
	int GetCount(void) const;
	// Get the maximum number of items from this class instance
	int GetMaxCount(void) const;
	//Loads the Data needed depending on item name
	void Load(void);

	// Get the texture ID
	unsigned int GetTextureID(void) const;

	// Name of the inventory item
	std::string sName;
	// The size of the image to render in the GUI
	glm::vec2 vec2Size;
	// Set the item type
	ITEM_TYPE itemtype;
	//Weapon Data only used for items which are melee or guns
	WeaponData* WData;
	//Gun Data only used for items which are melee or guns
	GunData* GData;


	// The amount of this item
	int iItemCount;
	// The total amount of this item
	int iItemMaxCount;
};
