/**
 Player2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Object2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

#include "Primitives/MeshBuilder.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CObject2D::CObject2D(void)
	: animatedSprites(NULL)
	, cMap2D(NULL)
	, ObjectID(NULL)
	, runtimeColour(glm::vec4(1.0f))
	, Idle(NULL)
	, Transition(NULL)

{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::vec2(0);

	// Initialise vecNumMicroSteps
	vec2NumMicroSteps = glm::vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);
	InteractedWith = false;
}
CObject2D::CObject2D(int newObjectID)
	: animatedSprites(NULL)
	, cMap2D(NULL)
	, ObjectID(NULL)
	, runtimeColour(glm::vec4(1.0f))
	, Idle(NULL)
	, Transition(NULL)
{
	ObjectID = newObjectID;

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::vec2(0);

	// Initialise vecNumMicroSteps
	vec2NumMicroSteps = glm::vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CObject2D::~CObject2D(void)
{

	if (animatedSprites)
	{
		delete animatedSprites;
		animatedSprites = NULL;
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

CMap2D* CObject2D::GetMap2D(void)
{
	return cMap2D;
}

CSettings* CObject2D::GetSettings(void)
{
	return cSettings;
}

void CObject2D::SetTransition(bool newState)
{
	Transition = newState;
}
void CObject2D::SetIdle(bool newState)
{
	Idle = newState;
}

bool CObject2D::GetIdle(void)
{
	return Idle;
}

void CObject2D::SetPlayervec2Index(glm::vec2 Playervec2Index)
{
	this->Playervec2Index = Playervec2Index;
}

void CObject2D::Interact(void)
{
	InteractedWith = true;
}

void CObject2D::SetruntimeColour(glm::vec4 newColour)
{
	runtimeColour = newColour;
}




/**
  @brief Initialise this instance
  */
bool CObject2D::Init(int x , int y)
{

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Set the position of the object to iRow and iCol
	vec2Index = glm::vec2(x, y);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	if (ObjectID == 24)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/ChestSpriteSheet.png", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/ChestSpriteSheet.png" << endl;
			return false;
		}
	}/*
	else if (ObjectID == 25)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/CoinsSpriteSheet.png", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/CoinsSpriteSheet.png" << endl;
			return false;
		}
	}*/
	else if (ObjectID == 26)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/KeySpriteSheet.png", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/KeySpriteSheet.png" << endl;
			return false;
		}
	}
	else if (ObjectID == 27)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/RuneSpriteSheet.png", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/RuneSpriteSheet.png" << endl;
			return false;
		}
	}
	else if (ObjectID == 28)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/DoorSpriteSheet.png", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/DoorSpriteSheet.png" << endl;
			return false;
		}
	}
	// Create the quad mesh for the player
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS:Create Animated Sprites and setup animation
	if (ObjectID == 24)
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
		animatedSprites->AddAnimation("active", 0, 0);
		animatedSprites->AddAnimation("transition", 1, 3);
		animatedSprites->AddAnimation("idle", 3, 3);
	}
	/*else if (ObjectID == 25)
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
		animatedSprites->AddAnimation("idle", 0, 0);
		animatedSprites->AddAnimation("active", 0, 3);
	}*/
	else if (ObjectID == 26)
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
		animatedSprites->AddAnimation("idle", 0, 0);
		animatedSprites->AddAnimation("active", 0, 3);
	}
	else if (ObjectID == 27)
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
		animatedSprites->AddAnimation("idle", 0, 0);
		animatedSprites->AddAnimation("active", 0, 3);
	}
	else if (ObjectID == 28)
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(4, 1, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
		animatedSprites->AddAnimation("active", 0, 0);
		animatedSprites->AddAnimation("transition", 1, 3);
		animatedSprites->AddAnimation("idle", 3, 3);
	}

	return true;
}

/**
 @brief Reset this instance
 */
bool CObject2D::Reset()
{
	return true;	
}

/**
 @brief Update this instance
 */
void CObject2D::Update(const double dElapsedTime)
{
	if (Idle == false)
	{
		animatedSprites->PlayAnimation("active", -1, 1.0f);
	}
	else if (ObjectID == 24 || ObjectID == 28)
	{
		if (Transition == true)
		{
			animatedSprites->PlayAnimation("transition", 1, 1.0f);
			Transition = false;
		}
		else
		{
			animatedSprites->PlayAnimation("idle", -1, 1.0f);
		}
	}
	
	
	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	{
		//if within viewing distance
		if ((abs(Playervec2Index.x - vec2Index.x) <= cSettings->VIEW_TILES_XAXIS * 0.5 + 1) && (abs(Playervec2Index.y - vec2Index.y) <= cSettings->VIEW_TILES_YAXIS * 0.5 + 1))
		{
			//Calculate Position of Entity on Screen
			glm::vec2 ScreenPos = glm::vec2(0, 0);

			//Check if Map View/Camera at Borders
			if (Playervec2Index.x < cSettings->VIEW_TILES_XAXIS * 0.5) // Left Side Border
			{
				ScreenPos.x = vec2Index.x + 1;
			}
			else if (Playervec2Index.x > (cSettings->NUM_TILES_XAXIS - (cSettings->VIEW_TILES_XAXIS * 0.5))) //Right Side Border
			{
				ScreenPos.x = cSettings->VIEW_TILES_XAXIS - (cSettings->NUM_TILES_XAXIS - vec2Index.x) + 1;
			}

			if (Playervec2Index.y > (cSettings->NUM_TILES_YAXIS - (cSettings->VIEW_TILES_YAXIS * 0.5))) //Top Side Border
			{
				ScreenPos.y = cSettings->VIEW_TILES_YAXIS - (cSettings->NUM_TILES_YAXIS - vec2Index.y) + 1;
			}
			else if (Playervec2Index.y < cSettings->VIEW_TILES_YAXIS * 0.5) //Bottom Side Border
			{
				ScreenPos.y = vec2Index.y + 1;
			}


			//If not at Border, Entity at Center of Screen displaced by x:1 y:1
			if (ScreenPos.x == 0)
			{
				ScreenPos.x = vec2Index.x + 1 - Playervec2Index.x + cSettings->VIEW_TILES_XAXIS * 0.5;
			}
			if (ScreenPos.y == 0)
			{
				ScreenPos.y = vec2Index.y + 1 - Playervec2Index.y + cSettings->VIEW_TILES_YAXIS * 0.5;
			}

			std::cout << "x: " << ScreenPos.x << " y: " << ScreenPos.y << std::endl;


			//Convert position to UV Coords
			vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, ScreenPos.x - 1, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
			vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, ScreenPos.y - 1, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
		}
		else
		{
			vec2UVCoordinate = glm::vec2(2, 2);
		}
	}
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CObject2D::PreRender(void)
{
	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CObject2D::Render(void)
{
	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtimeColour");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x,
													vec2UVCoordinate.y,
													0.0f));
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(runtimeColour));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);


	//Render the Player sprite
	glBindVertexArray(VAO);
	//quadMesh->Render();
	//CS: Render the animated sprites
	animatedSprites->Render();
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CObject2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}


