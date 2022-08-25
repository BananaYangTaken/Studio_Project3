#include "Mouse2D.h"
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
CMouse2D::CMouse2D(void)
	: animatedSprites(NULL)
	, runtimeColour(glm::vec4(1.0f))
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
CMouse2D::~CMouse2D(void)
{

	if (animatedSprites)
	{
		delete animatedSprites;
		animatedSprites = NULL;
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}
bool CMouse2D::Init()
{
	cPhysics2D.Init();
	
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	
	this->SetShader("Shader2D_Colour");
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Crosshair.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Crosshair.png" << endl;
		return false;
	}
	

	// Create the quad mesh for the player
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS:Create Animated Sprites and setup animation

	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 1, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("Mouse", 0, 0);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

}

/**
 @brief Reset this instance
 */
bool CMouse2D::Reset()
{
	return true;
}

/**
 @brief Update this instance
 */
void CMouse2D::Update(const double dElapsedTime)
{
	animatedSprites->PlayAnimation("Mouse", -1, 1);

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	//vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	//vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
	
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CMouse2D::PreRender(void)
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
void CMouse2D::Render(void)
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
void CMouse2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}


