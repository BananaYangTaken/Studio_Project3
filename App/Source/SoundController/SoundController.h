/**
 CSoundController
 @brief A class which manages the sound objects
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingletonTemplate
#include <DesignPatterns\SingletonTemplate.h>

// Include GLEW
#include <includes/irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

// Include string
#include <string>
// Include map storage
#include <map>
#include <vector>
using namespace std;

// Include SoundInfo class; it stores the sound and other information
#include "SoundInfo.h"

class CSoundController : public CSingletonTemplate<CSoundController>
{
	friend CSingletonTemplate<CSoundController>;
public:
	// Initialise this class instance
	bool Init(void);

	// Update
	bool Update(const double dElapsedTime);

	// Load a sound
	bool LoadSound(	string filename, 
					const int ID,
					const bool bPreload = true,
					const bool bIsLooped = false,
					CSoundInfo::SOUNDGROUP eSoundGroup = CSoundInfo::SOUNDGROUP::Player,
					CSoundInfo::SOUNDTYPE eSoundType = CSoundInfo::SOUNDTYPE::_2D,
					vec3df vec3dfSoundPos = vec3df(0.0f, 0.0f, 0.0f));

	void LoadSoundSystem(void);

	// Play a sound by its ID
	void PlaySoundByID(const int ID);

	// Stop a sound by its ID
	void StopSoundByID(const int ID);

	// Increase Master volume
	bool MasterVolumeIncrease(float volume = 1.f);
	// Decrease Master volume
	bool MasterVolumeDecrease(float volume = 1.f);
	
	float GetMasterVolume(void);

	// Increase volume of a ISoundSource
	bool VolumeIncrease(const int ID);
	// Decrease volume of a ISoundSource
	bool VolumeDecrease(const int ID);

	// Increase volume of a Sound Group
	bool SoundGroupVolumeIncrease(CSoundInfo::SOUNDGROUP eSoundGroup, float volume = 1.f);
	// Decrease volume of a Sound Group
	bool SoundGroupVolumeDecrease(CSoundInfo::SOUNDGROUP eSoundGroup, float volume = 1.f);
	float GetSoundGroupVolume(CSoundInfo::SOUNDGROUP eSoundGroup);

	// For 3D sounds only
	// Set Listener position
	void SetListenerPosition(const float x, const float y, const float z);
	// Set Listener direction
	void SetListenerDirection(const float x, const float y, const float z);

protected:
	// Constructor
	CSoundController(void);

	// Destructor
	virtual ~CSoundController(void);

	// Get an sound from this map
	CSoundInfo* GetSound(const int ID);
	// Remove an sound from this map
	bool RemoveSound(const int ID);
	// Get the number of sounds in this map
	int GetNumOfSounds(void) const;

	// The handler to the irrklang Sound Engine
	ISoundEngine* cSoundEngine;

	// The map of all the entity created
	std::map<int, CSoundInfo*> soundMap;

	//Sound Playlists
	std::vector<std::vector<int>> SoundSystem;
	//Volume Values
	float VolumeValue[5];


	// For 3D sound only: Listener position
	vec3df vec3dfListenerPos;
	// For 3D sound only: Listender view direction
	vec3df vec3dfListenerDir;
};

