#include "Projectile2D.h"
#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
#include "Primitives/MeshBuilder.h"
#include "Player2D_V2.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CProjectile2D::CProjectile2D(void)
	: animatedSprites(NULL)
	, runtimeColour(glm::vec4(1.0f))
	, Speed(1)
	, Direction(NULL)
	, Type(HOSTILE)
	, Projectile(BULLET)
	, Damage(1)
	, ExistenceTimer(0)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::vec2(0);

	// Initialise vecNumMicroSteps
	vec2NumMicroSteps = glm::vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	Rotation = 0;
}


/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CProjectile2D::~CProjectile2D(void)
{

	if (animatedSprites)
	{
		delete animatedSprites;
		animatedSprites = NULL;
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}
CProjectile2D::TYPE CProjectile2D::GetType(void)
{
	return Type;
}
float CProjectile2D::GetDamage(void)
{
	return Damage;
}
float CProjectile2D::GetExistenceTimer(void)
{
	return ExistenceTimer;
}
bool CProjectile2D::Init(glm::vec2 newvec2Index, glm::vec2 newvec2NumMicroSteps, float newRotation, float newSpeed, TYPE newType, PROJECTILE newProjectile, float newDamage)
{
	cPhysics2D.Init();
	Player = dynamic_cast<CEntity2D*>(CPlayer2D_V2::GetInstance());

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	this->Rotation = newRotation;
	Speed = newSpeed;
	Type = newType;
	Projectile = newProjectile;
	this->vec2Index = newvec2Index;
	Direction = cPhysics2D.RotateVec2(glm::vec2(1,0), newRotation);
	vec2NumMicroSteps.x = newvec2NumMicroSteps.x + Direction.x * cSettings->NUM_STEPS_PER_TILE_XAXIS * 0.6;
	vec2NumMicroSteps.y = newvec2NumMicroSteps.y + Direction.y * cSettings->NUM_STEPS_PER_TILE_YAXIS * 0.6;
	Damage = newDamage;

	this->SetShader("Shader2D_Colour");
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	if (Projectile == 0)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Bullet.png", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/Bullet.png" << endl;
			return false;
		}
	}
	else if (Projectile == 1)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Slash.png", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/Slash.png" << endl;
			return false;
		}
	}

	// Create the quad mesh for the player
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS:Create Animated Sprites and setup animation
	if (Projectile == 0)
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 1, (cSettings->TILE_WIDTH + cSettings->TILE_HEIGHT) * 0.5, (cSettings->TILE_WIDTH + cSettings->TILE_HEIGHT) * 0.5);
		animatedSprites->AddAnimation("Attack", 0, 0);
	}
	if (Projectile == 1)
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 1, (cSettings->TILE_WIDTH + cSettings->TILE_HEIGHT) * 0.5, (cSettings->TILE_WIDTH + cSettings->TILE_HEIGHT) * 0.5);
		animatedSprites->AddAnimation("Attack", 0, 0);
		ExistenceTimer = 0.1;
	}

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

}

/**
 @brief Reset this instance
 */
bool CProjectile2D::Reset()
{
	return true;
}

/**
 @brief Update this instance
 */
void CProjectile2D::Update(const double dElapsedTime)
{
	animatedSprites->PlayAnimation("Attack", -1, 1);
	vec2NumMicroSteps.x += Direction.x * Speed * dElapsedTime;
	vec2NumMicroSteps.y += Direction.y * Speed * dElapsedTime;
	
	if (Projectile == PROJECTILE::KNIFE)
	{
		if (ExistenceTimer > 0)
		{
			ExistenceTimer -= dElapsedTime;
		}
		else if (ExistenceTimer <= 0)
		{
			ExistenceTimer = 0;
		}
	}


	//Update vec2Index
	{
		if (vec2NumMicroSteps.x < 0)
		{
			--vec2Index.x;
			vec2NumMicroSteps.x += 15;
		}
		else if (vec2NumMicroSteps.x > 15)
		{
			++vec2Index.x;
			vec2NumMicroSteps.x = 0;
		}

		if (vec2NumMicroSteps.y < 0)
		{
			--vec2Index.y;
			vec2NumMicroSteps.y += 15;
		}
		else if (vec2NumMicroSteps.y > 15)
		{
			++vec2Index.y;
			vec2NumMicroSteps.y = 0;
		}
	}

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);
	
	// Update the UV Coordinates
	{
		//if within viewing distance
		if ((abs(Player->vec2Index.x - vec2Index.x) <= cSettings->VIEW_TILES_XAXIS * 0.5 + 1) && (abs(Player->vec2Index.y - vec2Index.y) <= cSettings->VIEW_TILES_YAXIS * 0.5 + 1))
		{
			//Calculate Position of Entity on Screen
			glm::vec2 ScreenPos = glm::vec2(0, 0);
			glm::vec2 TotalMicroStep = vec2NumMicroSteps;
			//Check if Map View/Camera at Borders
			if (Player->vec2Index.x < cSettings->VIEW_TILES_XAXIS * 0.5) // Left Side Border
			{
				ScreenPos.x = vec2Index.x + 1;
			}
			else if (Player->vec2Index.x > (cSettings->NUM_TILES_XAXIS - (cSettings->VIEW_TILES_XAXIS * 0.5))) //Right Side Border
			{
				ScreenPos.x = cSettings->VIEW_TILES_XAXIS - (cSettings->NUM_TILES_XAXIS - vec2Index.x) + 1;
			}
			else
			{
				TotalMicroStep.x = TotalMicroStep.x - Player->vec2NumMicroSteps.x;
			}

			if (Player->vec2Index.y > (cSettings->NUM_TILES_YAXIS - (cSettings->VIEW_TILES_YAXIS * 0.5))) //Top Side Border
			{
				ScreenPos.y = cSettings->VIEW_TILES_YAXIS - (cSettings->NUM_TILES_YAXIS - vec2Index.y) + 1;
			}
			else if (Player->vec2Index.y < cSettings->VIEW_TILES_YAXIS * 0.5) //Bottom Side Border
			{
				ScreenPos.y = vec2Index.y + 1;
			}
			else
			{
				TotalMicroStep.y = TotalMicroStep.y - Player->vec2NumMicroSteps.y;
			}


			//If not at Border, Entity at Center of Screen displaced by x:1 y:1
			if (ScreenPos.x == 0)
			{
				ScreenPos.x = vec2Index.x + 1 - Player->vec2Index.x + cSettings->VIEW_TILES_XAXIS * 0.5;
			}
			if (ScreenPos.y == 0)
			{
				ScreenPos.y = vec2Index.y + 1 - Player->vec2Index.y + cSettings->VIEW_TILES_YAXIS * 0.5;
			}

			//Convert position to UV Coords
			vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, ScreenPos.x - 1, false, (TotalMicroStep.x) * cSettings->MICRO_STEP_XAXIS);
			vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, ScreenPos.y - 1, false, (TotalMicroStep.y) * cSettings->MICRO_STEP_YAXIS);
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
void CProjectile2D::PreRender(void)
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
void CProjectile2D::Render(void)
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
	transform = glm::rotate(transform, Rotation, glm::vec3(0, 0, 1));
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
void CProjectile2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}


