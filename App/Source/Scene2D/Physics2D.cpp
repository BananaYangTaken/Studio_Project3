/**
 CPhysics2D
 @brief A class to calculate SUVAT equations for the game
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Physics2D.h"
#include "../MyMath.h"
#include "GameControl/Settings.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPhysics2D::CPhysics2D(void)
	: v2InitialVelocity(glm::vec2(0.0f))
	, v2FinalVelocity(glm::vec2(0.0f))
	, v2Acceleration(glm::vec2(0.0f))
	, v2Displacement(glm::vec2(0.0f))
	, v2PrevDisplacement(glm::vec2(0.0f))
	, fTime(0.0f)
	, sCurrentStatus(STATUS::IDLE)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPhysics2D::~CPhysics2D(void)
{
}

/**
@brief Init Initialise this instance
*/ 
bool CPhysics2D::Init(void)
{
	// Reset these variables
	v2InitialVelocity = glm::vec2(0.0f);
	v2FinalVelocity = glm::vec2(0.0f);
	//v2Acceleration = glm::vec2(0.0f);		// Acceleration does not need to be reset here.
	v2Displacement = glm::vec2(0.0f);
	v2PrevDisplacement = glm::vec2(0.0f);
	fTime = 0.0f;
	return true;
}

// Set methods
// Set Initial velocity
void CPhysics2D::SetInitialVelocity(const glm::vec2 v2InitialVelocity)
{
	this->v2InitialVelocity = v2InitialVelocity;	// Initial velocity
}

// Set Final velocity
void CPhysics2D::SetFinalVelocity(const glm::vec2 v2FinalVelocity)
{
	this->v2FinalVelocity = v2FinalVelocity;		// Final velocity
}

// Set Acceleration
void CPhysics2D::SetAcceleration(const glm::vec2 v2Acceleration)
{
	this->v2Acceleration = v2Acceleration;		// Acceleration
}

// Set Displacement
void CPhysics2D::SetDisplacement(const glm::vec2 v2Displacement)
{
	this->v2Displacement = v2Displacement;		// Displacement
}

// Set Time
void CPhysics2D::SetTime(const float fTime)
{
	this->fTime = fTime;					// Time
}

// Set Status
void CPhysics2D::SetStatus(const STATUS sStatus)
{
	// If there is a change in status, then reset to default values
	if (sCurrentStatus != sStatus)
	{
		// Reset to default values
		Init();

		// Store the new status
		sCurrentStatus = sStatus;
	}
}

// Get methods
// Get Initial velocity
glm::vec2 CPhysics2D::GetInitialVelocity(void) const
{
	return v2InitialVelocity;	// Initial velocity
}

// Get Final velocity
glm::vec2 CPhysics2D::GetFinalVelocity(void) const
{
	return v2FinalVelocity;		// Final velocity
}

// Get Acceleration
glm::vec2 CPhysics2D::GetAcceleration(void) const
{
	return v2Acceleration;		// Acceleration
}

// Get Displacement
glm::vec2 CPhysics2D::GetDisplacement(void) const
{
	return v2Displacement;		// Displacement
}

// Get Delta Displacement
glm::vec2 CPhysics2D::GetDeltaDisplacement(void) const
{
	return v2Displacement - v2PrevDisplacement;		// Delta Displacement
}

// Get Time
float CPhysics2D::GetTime(void) const
{
	return fTime;					// Time
}

// Get Status
CPhysics2D::STATUS CPhysics2D::GetStatus(void) const
{
	return sCurrentStatus;
}

// Update
void CPhysics2D::Update(void)
{
	// If the player is in IDLE mode, 
	// then we don't calculate further
	if (sCurrentStatus == IDLE)
		return;

	// Store the previous displacement values
	v2PrevDisplacement = v2Displacement;

	/** This set of codes is assuming constant Initial Velocity 
	    and we extract the delta displacement only. The upward movement is constant.
	// Calculate the displacement
	v2Displacement = v2InitialVelocity * fTime + 0.5f * v2Gravity * fTime * fTime;
	*/

	/** This set of codes updates Initial Velocity after every update
		and we extract the displacement only. The upward movement varies.
	*/
	// Calculate the displacement
	v2Displacement = v2InitialVelocity * fTime;

	// Update v2InitialVelocity
	v2InitialVelocity = v2InitialVelocity + v2Gravity * fTime;
}

// Add elapsed time
void CPhysics2D::AddElapsedTime(const float fElapseTime)
{
	fTime += fElapseTime;
}

// Calculate the distance between two vec2 varables
float CPhysics2D::CalculateDistance(glm::vec2 source, glm::vec2 destination, char axis, bool micro, glm::vec2 sourcemicro, glm::vec2 destinationmicro)
{
	if (!micro)
	{
		if (axis == 'n')
		{
			return glm::length(destination - source);
		}
		else if (axis == 'x')
		{
			return abs(destination.x - source.x);
		}
		else if (axis == 'y')
		{
			return abs(destination.y - source.y);
		}
	}
	else
	{
		if (axis == 'n')
		{
			glm::vec2 destinationMicro;
			destinationMicro.x = destination.x * CSettings::GetInstance()->NUM_TILES_XAXIS + destinationmicro.x;
			destinationMicro.y = destination.y * CSettings::GetInstance()->NUM_TILES_YAXIS + destinationmicro.y;
			glm::vec2 sourceMicro;
			sourceMicro.x = source.x * CSettings::GetInstance()->NUM_TILES_XAXIS + sourcemicro.x;
			sourceMicro.y = source.y * CSettings::GetInstance()->NUM_TILES_YAXIS + sourcemicro.y;
			return glm::length(destinationMicro - sourceMicro);
		}
		else if (axis == 'x')
		{
			return abs(destination.x * CSettings::GetInstance()->NUM_TILES_XAXIS + destinationmicro.x - source.x * CSettings::GetInstance()->NUM_TILES_XAXIS + sourcemicro.x);
		}
		else if (axis == 'y')
		{
			return abs(destination.y * CSettings::GetInstance()->NUM_TILES_YAXIS + destinationmicro.y - source.y * CSettings::GetInstance()->NUM_TILES_YAXIS + sourcemicro.y);
		}
	}
}

void CPhysics2D::Normalize(glm::vec2 vec2)
{
	float d = sqrt(vec2.x* vec2.x + vec2.y* vec2.y);
	if (d <= Math::EPSILON && -d <= Math::EPSILON)
		vec2 = glm::vec2(0, 0);
	vec2.x /= d;
	vec2.y /= d;
}

float CPhysics2D::CalculateRotation(glm::vec2 Origin, glm::vec2 DefaultPos, glm::vec2 NewPos)
{
	glm::vec2 OriginalPoint = DefaultPos - Origin;
	glm::vec2 TransformedPoint = NewPos - Origin;
	float XtoYRatio = CSettings::GetInstance()->VIEW_TILES_XAXIS / CSettings::GetInstance()->VIEW_TILES_YAXIS;
	OriginalPoint.x *= XtoYRatio;
	TransformedPoint.x *= XtoYRatio;
	Normalize(OriginalPoint);
	Normalize(TransformedPoint);
	float Radian = atan2(TransformedPoint.y * OriginalPoint.x - TransformedPoint.x * OriginalPoint.y,TransformedPoint.x*OriginalPoint.x + TransformedPoint.y + OriginalPoint.y );
	if (Radian < 0)
	{
		Radian = -Radian;
	}
	else if (Radian > 0)
	{
		Radian = Math::TWO_PI - Radian;
	}
	else
	{
		Radian = 0;
	}
	Radian = Math::TWO_PI - Radian;
	return Radian;
}

glm::vec2 CPhysics2D::RotateVec2(glm::vec2 vec2, float Radian)
{
	glm::vec2 Temp;
	Temp = glm::vec2((vec2.x * cos(Radian)) - (vec2.y * sin(Radian))
		, (vec2.y * cos(Radian)) + (vec2.x * sin(Radian)));
	Temp.x = ceil(Temp.x * pow(10, 7)) * pow(10, -7);
	Temp.y = ceil(Temp.y * pow(10, 7)) * pow(10, -7);
	return Temp;
}


// PrintSelf
void CPhysics2D::PrintSelf(void)
{
	cout << "CPhysics2D::PrintSelf()" << endl;
	cout << "v2InitialVelocity\t=\t" << v2InitialVelocity.x << ", " << v2InitialVelocity.y << endl;
	cout << "v2FinalVelocity\t=\t" << v2FinalVelocity.x << ", " << v2FinalVelocity.y << endl;
	cout << "v2Acceleration\t=\t" << v2Acceleration.x << ", " << v2Acceleration.y << endl;
	cout << "v2Displacement\t=\t" << v2Displacement.x << ", " << v2Displacement.y << endl;
	cout << "fTime\t=\t" << fTime << endl;
}
