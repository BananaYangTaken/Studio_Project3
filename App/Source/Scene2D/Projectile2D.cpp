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

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CProjectile2D::CProjectile2D(void)
	: animatedSprites(NULL)
	, runtimeColour(glm::vec4(1.0f))
	, Direction(DOWN)
	, Speed(1)
	, Type(HOSTILE)
	, Projectile(ARROW)
{
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
bool CProjectile2D::Init(glm::vec2 vec2Index, glm::vec2 vec2NumMicroSteps, unsigned int Direction, float Speed, TYPE Type, PROJECTILE Projectile)
{

	// Load the sounds into CSoundController
	cSoundController = CSoundController::GetInstance();

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	this->Direction = Direction;
	this->Speed = Speed;
	this->Type = Type;
	this->Projectile = Projectile;
	this->vec2Index = vec2Index;
	this->vec2NumMicroSteps = vec2NumMicroSteps;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	if (Projectile == 0)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/ArrowSpriteSheet.png", true);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/ArrowSpriteSheet.png" << endl;
			return false;
		}
	}

	// Create the quad mesh for the player
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS:Create Animated Sprites and setup animation
	if (Projectile == 0)
	{
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
		animatedSprites->AddAnimation("Up", 0, 0);
		animatedSprites->AddAnimation("Left", 1, 1);
		animatedSprites->AddAnimation("Down", 2, 2);
		animatedSprites->AddAnimation("Right", 3, 3);
	}

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	CSoundController::GetInstance()->PlaySoundByID(7);
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
	if (Direction == 0)
	{
		animatedSprites->PlayAnimation("Left", -1, 1);
		vec2NumMicroSteps.x -= Speed * dElapsedTime;
		//Calculate New Position
		if (vec2Index.x >= 0)
		{
			vec2NumMicroSteps.x -= Speed * dElapsedTime * cSettings->NUM_STEPS_PER_TILE_XAXIS;

			if (vec2NumMicroSteps.x < 0)
			{
				vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - Speed * dElapsedTime;
				vec2Index.x--;
			}
		}
	}
	else if (Direction == 1)
	{
		animatedSprites->PlayAnimation("Right", -1, 1);
		vec2NumMicroSteps.x += Speed * dElapsedTime;
		//Calculate New Position
		if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			vec2NumMicroSteps.x += Speed * dElapsedTime * cSettings->NUM_STEPS_PER_TILE_XAXIS;

			if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
			{
				vec2NumMicroSteps.x = 0;
				vec2Index.x++;
			}
		}
	}
	else if (Direction == 2)
	{
		animatedSprites->PlayAnimation("Up", -1, 1);
		//Calculate New Position
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y += Speed * dElapsedTime * cSettings->NUM_STEPS_PER_TILE_YAXIS;

			if (vec2NumMicroSteps.y >= cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				vec2NumMicroSteps.y = 0;
				vec2Index.y++;
			}
		}
	}
	else if (Direction == 3)
	{
		animatedSprites->PlayAnimation("Down", -1, 1);
		vec2NumMicroSteps.y -= Speed * dElapsedTime;
		//Calculate New Position
		if (vec2Index.y >= 0)
		{
			vec2NumMicroSteps.y -= Speed * dElapsedTime * cSettings->NUM_STEPS_PER_TILE_YAXIS;

			if (vec2NumMicroSteps.y < 0)
			{
				vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - Speed * dElapsedTime;
				vec2Index.y--;
			}
		}
	}

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);
	

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
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


