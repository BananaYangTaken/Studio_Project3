/**
 Map2D
 @brief A class which manages the map in the game
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Map2D.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include Filesystem
#include "System\filesystem.h"
// Include ImageLoader
#include "System\ImageLoader.h"
#include "Primitives/MeshBuilder.h"

#include <iostream>
#include <vector>
using namespace std;

// For AStar PathFinding
using namespace std::placeholders;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CMap2D::CMap2D(void)
	: uiCurLevel(0)
	, quadMesh(NULL)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CMap2D::~CMap2D(void)
{
	// Dynamically deallocate the 3D array used to store the map information
	for (unsigned int uiLevel = 0; uiLevel < uiNumLevels; uiLevel++)
	{
		for (unsigned int iLayer = 0; iLayer < 2; iLayer++)
		{
			for (unsigned int iRow = 0; iRow < cSettings->NUM_TILES_YAXIS; iRow++)
			{
				delete[] arrMapInfo[uiLevel][iLayer][iRow];
			}
			delete[] arrMapInfo[uiLevel][iLayer];
		}
		delete [] arrMapInfo[uiLevel];
	}
	delete[] arrMapInfo;

	if (quadMesh)
	{
		delete quadMesh;
		quadMesh = NULL;
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// Set this to NULL since it was created elsewhere, so we let it be deleted there.
	cSettings = NULL;
	// Delete AStar lists
	DeleteAStarLists();
}

/**
@brief Init Initialise this instance
*/ 
bool CMap2D::Init(	const unsigned int uiNumLevels,const unsigned int uiNumRows,const unsigned int uiNumCols)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();
	// Create the arrMapInfo and initialise to 0
	// Start by initialising the number of levels
	arrMapInfo = new Grid*** [uiNumLevels];
	for (unsigned uiLevel = 0; uiLevel < uiNumLevels; uiLevel++)
	{
		arrMapInfo[uiLevel] = new Grid ** [2];
		for (unsigned uiLayer = 0; uiLayer < 2; uiLayer++)
		{
			arrMapInfo[uiLevel][uiLayer] = new Grid * [uiNumRows];
			for (unsigned uiRow = 0; uiRow < uiNumRows; uiRow++)
			{
				arrMapInfo[uiLevel][uiLayer][uiRow] = new Grid[uiNumCols];
				for (unsigned uiCol = 0; uiCol < uiNumCols; uiCol++)
				{
					arrMapInfo[uiLevel][uiLayer][uiRow][uiCol].value = 0;
				}
			}
		}
	}
	Displacement = glm::vec2(0, 0);
	// Store the map sizes in cSettings
	uiCurLevel = 0;
	this->uiNumLevels = uiNumLevels;
	cSettings->NUM_TILES_XAXIS = uiNumCols;
	cSettings->NUM_TILES_YAXIS = uiNumRows;
	cSettings->UpdateSpecifications();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	//Load Texture
	{
		//load Horizontal Wall texture
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Hor_Wall.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Hor_Wall.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(101, iTextureID));
			}
		}
		//load Vertical Wall texture
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Ver_Wall.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Ver_Wall.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(102, iTextureID));
			}
		}
		//load Horizontal Window texture
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Hor_Window.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Hor_Window.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(50, iTextureID));
			}
		}
		//load Vertical Window texture
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Ver_Window.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Ver_Window.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(54, iTextureID));
			}
		}
		//load Wall texture
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Wall.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Wall.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(105, iTextureID));
			}
		}
		//load Wall texture
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Floor.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Floor.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(21, iTextureID));
			}
		}
		//load Road texture
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Road.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Road.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(22, iTextureID));
			}
		}
		//load Divider texture
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Divider.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Divider.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(23, iTextureID));
			}
		}
		//load Divider texture
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Fence.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Fence.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(106, iTextureID));
			}
		}
		//Load Small crate
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/lootcrateSmall.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/lootcrateLarge.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(110, iTextureID));
			}
		}
		//Load Barbwire for player house
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Barbwire.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Barbwire.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(24, iTextureID));
			}
		}
		//Load lvl2 window
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Hor_WindowL2.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Hor_WindowL2.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(119, iTextureID));
			}
		}
		//Load lvl3 window
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Hor_WindowL3.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Hor_WindowL3.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(120, iTextureID));
			}
		}

		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/CTable.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/CTable.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(113, iTextureID));
			}
		}
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Yellow_Wall.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Envi/Yellow_Wall.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(115, iTextureID));
			}
		}


		//Turret's bases
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Lv1TurretBase.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Lv1TurretBase.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(116, iTextureID));
			}
		}
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Lv2TurretBase.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Lv2TurretBase.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(117, iTextureID));
			}
		}
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Lv3TurretBase.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Lv3TurretBase.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(118, iTextureID));
			}
		}

		//Items
		//bandages
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Bandage.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Bandage.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(30, iTextureID));
			}
		}
		//Blueprint
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Blueprint.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Blueprint.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(32, iTextureID));
			}
		}
		//Fabric
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Fabric.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Fabric.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(33, iTextureID));
			}
		}
		//Hard Wood
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Hard Wood.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Hard Wood.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(34, iTextureID));
			}
		}
		//Medkit
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Medkit.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Medkit.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(35, iTextureID));
			}
		}
		//Pistol Bullets
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Pistol Bullets.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Pistol Bullets.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(36, iTextureID));
			}
		}
		//Pistol
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Pistol.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Pistol.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(37, iTextureID));
			}
		}
		//Rifle bullet
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Rifle Bullets.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Rifle Bullets.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(38, iTextureID));
			}
		}
		//Rifle
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Rifle.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Pistol.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(39, iTextureID));
			}
		}
		//Scrap metal
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Scrap Metal.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Scrap Metal.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(40, iTextureID));
			}
		}
		//Stone Ore
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Stone Ore.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Stone Ore.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(41, iTextureID));
			}
		}
		//Yellow Keycard
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Yellow Keycard.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Yellow Keycard.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(42, iTextureID));
			}
		}
		//Flashlight
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/Flashlight.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Flashlight.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(43, iTextureID));
			}
		}
		//muzzleDevice
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/muzzleDevice.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/muzzleDevice.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(44, iTextureID));
			}
		}
		//extendedmag
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Iteme/extendedmag.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/extendedmag.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(45, iTextureID));
			}
		}
		//Ver Wall Top
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Ver_Wall_Top.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Ver_Wall_Top.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(123, iTextureID));
			}
		}

		//Hor Wall Right
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Hor_Wall_Right.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Hor_Wall_Right.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(122, iTextureID));
			}
		}

		//Ver Wall Bot
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Ver_Wall_Bot.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Ver_Wall_Bot.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(124, iTextureID));
			}
		}

		//Hor Wall Left
		{
			iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Envi/Hor_Wall_Left.tga", true);
			if (iTextureID == 0)
			{
				cout << "Unable to load Image/Iteme/Hor_Wall_Left.tga" << endl;
				return false;
			}
			else
			{
				// Store the texture ID into MapOfTextureIDs
				MapOfTextureIDs.insert(pair<int, int>(121, iTextureID));
			}
		}
	}
	

	// Initialise the variables for AStar
	m_weight = 1;
	m_startPos = glm::vec2(0, 0);
	m_targetPos = glm::vec2(0, 0);
	//m_size = cSettings->NUM_TILES_YAXIS* cSettings->NUM_TILES_XAXIS;

	m_nrOfDirections = 4;
	m_directions = { { -1, 0 }, { 1, 0 }, { 0, 1 }, { 0, -1 },
						{ -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 } };

	// Resize these 2 lists
	m_cameFromList.resize(cSettings->NUM_TILES_YAXIS* cSettings->NUM_TILES_XAXIS);
	m_closedList.resize(cSettings->NUM_TILES_YAXIS* cSettings->NUM_TILES_XAXIS, false);

	return true;
}

/**
@brief Update Update this instance
*/
void CMap2D::Update(const double dElapsedTime)
{
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CMap2D::PreRender(void)
{
	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render Render this instance
 */
void CMap2D::Render(void)
{
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	
	Displacement =  glm::vec2(0,0);
	Displacement.x = vec2Index.x - cSettings->VIEW_TILES_XAXIS * 0.5;
	if (Displacement.x < 0)
	{
		Displacement.x = 0;
		vec2NumMicroSteps.x = 0;
	}
	else if (Displacement.x > cSettings->NUM_TILES_XAXIS - cSettings->VIEW_TILES_XAXIS - 1)
	{
		Displacement.x = cSettings->NUM_TILES_XAXIS - cSettings->VIEW_TILES_XAXIS;
		vec2NumMicroSteps.x = 0;
	}
	
	Displacement.y = cSettings->NUM_TILES_YAXIS - vec2Index.y - cSettings->VIEW_TILES_YAXIS * 0.5;
	if (Displacement.y < 1)
	{
		Displacement.y = 0;
		vec2NumMicroSteps.y = 0;
	}
	else if (Displacement.y > cSettings->NUM_TILES_YAXIS - cSettings->VIEW_TILES_YAXIS )
	{
		Displacement.y = cSettings->NUM_TILES_YAXIS - cSettings->VIEW_TILES_YAXIS;
		vec2NumMicroSteps.y = 0;
	}
	
	// Render
	for (unsigned int uiRow = (Displacement.y - 1) < 0? Displacement.y: Displacement.y-1; uiRow < Displacement.y + cSettings->VIEW_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = Displacement.x; uiCol < Displacement.x + cSettings->VIEW_TILES_XAXIS+1; uiCol++)
		{
			transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			transform = glm::translate(transform, glm::vec3(cSettings->ConvertIndexToUVSpace(cSettings->x, uiCol - Displacement.x, false, -cSettings->MICRO_STEP_XAXIS * vec2NumMicroSteps.x),
				cSettings->ConvertIndexToUVSpace(cSettings->y, uiRow - Displacement.y, true, -cSettings->MICRO_STEP_YAXIS * vec2NumMicroSteps.y),
				0.0f));

			//transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

			// Update the shaders with the latest transform
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
			// Render a tile
			RenderTile(0, uiRow, uiCol);
			RenderTile(1, uiRow, uiCol);
		}
	}
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CMap2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

// Set the specifications of the map
void CMap2D::SetNumTiles(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	// Check if the value is valid
	if (uiValue <= 0)
	{
		cout << "CMap2D::SetNumTiles() : value must be more than 0" << endl;
		return;
	}

	if (sAxis == CSettings::x)
	{
		cSettings->NUM_TILES_XAXIS = uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::y)
	{
		cSettings->NUM_TILES_YAXIS = uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::z)
	{
		// Not used in here
	}
	else
	{
		cout << "Unknown axis" << endl;
	}
}

// Set the specifications of the map
void CMap2D::SetNumSteps(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	// Check if the value is valid
	if (uiValue <= 0)
	{
		cout << "CMap2D::SetNumSteps() : value must be more than 0" << endl;
		return;
	}

	if (sAxis == CSettings::x)
	{
		cSettings->NUM_STEPS_PER_TILE_XAXIS = (float)uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::y)
	{
		cSettings->NUM_STEPS_PER_TILE_YAXIS = (float)uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::z)
	{
		// Not used in here
	}
	else
	{
		cout << "Unknown axis" << endl;
	}
}

/**
 @brief Set the value at certain indices in the arrMapInfo
 @param iRow A const int variable containing the row index of the element to set to
 @param iCol A const int variable containing the column index of the element to set to
 @param iValue A const int variable containing the value to assign to this arrMapInfo
 */
void CMap2D::SetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert, const unsigned int uiCurLayer)
{
	//Only edit foreground
	if (bInvert)		
		arrMapInfo[uiCurLevel][uiCurLayer][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value = iValue;
	else
		arrMapInfo[uiCurLevel][uiCurLayer][uiRow][uiCol].value = iValue;
}

/**
 @brief Get the value at certain indices in the arrMapInfo
 @param iRow A const int variable containing the row index of the element to get from
 @param iCol A const int variable containing the column index of the element to get from
 @param bInvert A const bool variable which indicates if the row information is inverted
 */
int CMap2D::GetMapInfo(const unsigned int uiRow, const int unsigned uiCol, const bool bInvert) const
{
	//Only edit foreground
	if (bInvert)
		return arrMapInfo[uiCurLevel][1][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value;
	else							 
		return arrMapInfo[uiCurLevel][1][uiRow][uiCol].value;
}

bool CMap2D::CheckValue(const unsigned int uiRow, const unsigned int uiCol, const unsigned int ObjID, const unsigned int uiCurLayer)
{
	return arrMapInfo[uiCurLevel][uiCurLayer][uiRow][uiCol].value == ObjID;
}

Grid**** CMap2D::GetarrMapInfo(void)
{
	return arrMapInfo;
}



/**
 @brief Load a map
 */ 
bool CMap2D::LoadMap(string filename, const unsigned int uiCurLevel, const unsigned int uiCurLayer)
{
	doc = rapidcsv::Document(FileSystem::getPath(filename).c_str());

	// Check if the sizes of CSV data matches the declared arrMapInfo sizes
	if ((cSettings->NUM_TILES_XAXIS != (unsigned int)doc.GetColumnCount()) ||
		(cSettings->NUM_TILES_YAXIS != (unsigned int)doc.GetRowCount()))
	{
		cout << "Sizes of CSV map does not match declared arrMapInfo sizes." << endl;
		return false;
	}

	

	for (unsigned int uiCurLayer = 0; uiCurLayer < 2; uiCurLayer++)
	{
		// Read the rows and columns of CSV data into arrMapInfo
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			// Read a row from the CSV file
			std::vector<std::string> row = doc.GetRow<std::string>(uiRow);

			// Load a particular CSV value into the arrMapInfo
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
			{
				arrMapInfo[uiCurLevel][uiCurLayer][uiRow][uiCol].value = (int)stoi(row[uiCol]);
			}
		}
	}

	if (MorethanOneInstance(200, uiCurLevel) == true)
	{
		std::cout << "More than 1 player exists on map" << std::endl;
		return false;
	}

	return true;
}

/**
 @brief Save the tilemap to a text file
 @param filename A string variable containing the name of the text file to save the map to
 */
bool CMap2D::SaveMap(string filename, const unsigned int uiCurLevel)
{
	// Update the rapidcsv::Document from arrMapInfo
	for (unsigned int uiLayer = 0; uiLayer < 2; uiLayer++)
	{
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
			{
				doc.SetCell(uiCol, uiRow, arrMapInfo[uiCurLevel][uiLayer][uiRow][uiCol].value);
			}
			cout << endl;
		}
	}
	// Save the rapidcsv::Document to a file
	doc.Save(FileSystem::getPath(filename).c_str());

	return true;
}

/**
@brief Find the indices of a certain value in arrMapInfo
@param iValue A const int variable containing the row index of the found element
@param iRow A const int variable containing the row index of the found element
@param iCol A const int variable containing the column index of the found element
@param bInvert A const bool variable which indicates if the row information is inverted
*/
bool CMap2D::FindValue(const int iValue, unsigned int& uirRow, unsigned int& uirCol, const bool bInvert, const unsigned int uiCurLayer)
{
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			if (arrMapInfo[uiCurLevel][uiCurLayer][uiRow][uiCol].value == iValue)
			{
				if (bInvert)
					uirRow = cSettings->NUM_TILES_YAXIS - uiRow - 1;
				else
					uirRow = uiRow;
				uirCol = uiCol;
				return true;	// Return true immediately if the value has been found
			}
		}
	}
	return false;
}

bool CMap2D::MorethanOneInstance(unsigned int EntityNum, unsigned int uiCurLevel)
{
	int EntityCount = 0;
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			if (arrMapInfo[uiCurLevel][1][uiRow][uiCol].value == EntityNum)
			{
				EntityCount++;
			}
		}
	}
	if (EntityCount > 1)
	{ 
		return true;
	}
	else
	{
		return false;
	}
}

/**
 @brief Set current level
 */
void CMap2D::SetCurrentLevel(unsigned int uiCurLevel)
{
	if (uiCurLevel < uiNumLevels)
	{
		this->uiCurLevel = uiCurLevel;
	}
}
/**
 @brief Get current level
 */
unsigned int CMap2D::GetCurrentLevel(void) const
{
	return uiCurLevel;
}
 
/**
 @brief Render a tile at a position based on its tile index
 @param iRow A const int variable containing the row index of the tile
 @param iCol A const int variable containing the column index of the tile
 */
void CMap2D::RenderTile(const unsigned int uiLayer, const unsigned int uiRow, const unsigned int uiCol)
{
	if ((arrMapInfo[uiCurLevel][uiLayer][uiRow][uiCol].value > 0)
		&& (arrMapInfo[uiCurLevel][uiLayer][uiRow][uiCol].value <200) 
		&& (arrMapInfo[uiCurLevel][uiLayer][uiRow][uiCol].value < 25 
		|| arrMapInfo[uiCurLevel][uiLayer][uiRow][uiCol].value > 28)
		&& arrMapInfo[uiCurLevel][uiLayer][uiRow][uiCol].value != 103
		&& arrMapInfo[uiCurLevel][uiLayer][uiRow][uiCol].value != 104)
	{
		//if (arrMapInfo[uiCurLevel][uiRow][uiCol].value < 3)
			//std::cout << (arrMapInfo[uiCurLevel][uiRow][uiCol].value) << std::endl;
			glBindTexture(GL_TEXTURE_2D, MapOfTextureIDs.at(arrMapInfo[uiCurLevel][uiLayer][uiRow][uiCol].value));

			glBindVertexArray(VAO);
			//CS: Render the tile
			quadMesh->Render();
			glBindVertexArray(0);		
	}
}


/**
 @brief Find a path
 */
std::vector<glm::vec2> CMap2D::PathFind(const glm::vec2& startPos, const glm::vec2& targetPos, HeuristicFunction heuristicFunc, int weight)
{
	// Check if the startPos and targetPost are blocked
	if (isBlocked(startPos.y, startPos.x) ||
		(isBlocked(targetPos.y, targetPos.x)))
	{
		cout << "Invalid start or target position." << endl;
		// Return an empty path
		std::vector<glm::vec2> path;
		return path;
	}

	// Set up the variables and lists
	m_startPos = startPos;
	m_targetPos = targetPos;
	m_weight = weight;
	m_heuristic = std::bind(heuristicFunc, _1, _2, _3);

	// Reset AStar lists
	ResetAStarLists();

	// Add the start pos to 2 lists
	m_cameFromList[ConvertTo1D(m_startPos)].parent = m_startPos;
	m_openList.push(Grid(m_startPos, 0));

	unsigned int fNew, gNew, hNew;
	glm::vec2 currentPos;

	// Start the path finding...
	while (!m_openList.empty())
	{
		// Get the node with the least f value
		currentPos = m_openList.top().pos;
		//cout << endl << "*** New position to check: " << currentPos.x << ", " << currentPos.y << endl;
		//cout << "*** targetPos: " << m_targetPos.x << ", " << m_targetPos.y << endl;

		// If the targetPos was reached, then quit this loop
		if (currentPos == m_targetPos)
		{
			//cout << "=== Found the targetPos: " << m_targetPos.x << ", " << m_targetPos.y << endl;
			while (m_openList.size() != 0)
				m_openList.pop();
			break;
		}

		m_openList.pop();
		m_closedList[ConvertTo1D(currentPos)] = true;

		// Check the neighbors of the current node
		for (unsigned int i = 0; i < m_nrOfDirections; ++i)
		{
			const auto neighborPos = currentPos + m_directions[i];
			const auto neighborIndex = ConvertTo1D(neighborPos);

			//cout << "\t#" << i << ": Check this: " << neighborPos.x << ", " << neighborPos.y << ":\t";
			if (!isValid(neighborPos) ||
				isBlocked(neighborPos.y, neighborPos.x) ||
				m_closedList[neighborIndex] == true)
			{
				//cout << "This position is not valid. Going to next neighbour." << endl;
				continue;
			}

			gNew = m_cameFromList[ConvertTo1D(currentPos)].g + 1;
			hNew = m_heuristic(neighborPos, m_targetPos, m_weight);
			fNew = gNew + hNew;

			if (m_cameFromList[neighborIndex].f == 0 || fNew < m_cameFromList[neighborIndex].f)
			{
				//cout << "Adding to Open List: " << neighborPos.x << ", " << neighborPos.y;
				//cout << ". [ f : " << fNew << ", g : " << gNew << ", h : " << hNew << "]" << endl;
				m_openList.push(Grid(neighborPos, fNew));
				m_cameFromList[neighborIndex] = { neighborPos, currentPos, fNew, gNew, hNew };
			}
			else
			{
				//cout << "Not adding this" << endl;
			}
		}
		//system("pause");
	}

	return BuildPath();
}

/**
 @brief Build a path
 */
std::vector<glm::vec2> CMap2D::BuildPath() const
{
	std::vector<glm::vec2> path;
	auto currentPos = m_targetPos;
	auto currentIndex = ConvertTo1D(currentPos);

	while (!(m_cameFromList[currentIndex].parent == currentPos))
	{
		path.push_back(currentPos);
		currentPos = m_cameFromList[currentIndex].parent;
		currentIndex = ConvertTo1D(currentPos);
	}

	// If the path has only 1 entry, then it is the the target position
	if (path.size() == 1)
	{
		// if m_startPos is next to m_targetPos, then having 1 path point is OK
		if (m_nrOfDirections == 4)
		{
			if (abs(m_targetPos.y - m_startPos.y) + abs(m_targetPos.x - m_startPos.x) > 1)
				path.clear();
		}
		else
		{
			if (abs(m_targetPos.y - m_startPos.y) + abs(m_targetPos.x - m_startPos.x) > 2)
				path.clear();
			else if (abs(m_targetPos.y - m_startPos.y) + abs(m_targetPos.x - m_startPos.x) > 1)
				path.clear();
		}
	}
	else
		std::reverse(path.begin(), path.end());

	return path;
}

/**
 @brief Toggle the checks for diagonal movements
 */
void CMap2D::SetDiagonalMovement(const bool bEnable)
{
	m_nrOfDirections = (bEnable) ? 8 : 4;
}

/**
 @brief Print out the details about this class instance in the console
 */
void CMap2D::PrintSelf(void) const
{

	for (unsigned uiLevel = 0; uiLevel < uiNumLevels; uiLevel++)
	{
		cout << "Level: " << uiLevel << endl;
		for (unsigned uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			for (unsigned uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
			{
				cout.fill('0');
				cout.width(3);
				cout << arrMapInfo[uiLevel][1][uiRow][uiCol].value;
				if (uiCol != cSettings->NUM_TILES_XAXIS - 1)
					cout << ", ";
				else
					cout << endl;
			}
		}
	}

	cout << "m_openList: " << m_openList.size() << endl;
	cout << "m_cameFromList: " << m_cameFromList.size() << endl;
	cout << "m_closedList: " << m_closedList.size() << endl;

	cout << "===== AStar::PrintSelf() =====" << endl;
}

glm::vec2 CMap2D::GetDisplacement(void)
{
	return Displacement;
}

/**
 @brief Check if a position is valid
 */
bool CMap2D::isValid(const glm::vec2& pos) const
{
	//return (pos.x >= 0) && (pos.x < m_dimensions.x) &&
	//	(pos.y >= 0) && (pos.y < m_dimensions.y);
	return (pos.x >= 0) && (pos.x < cSettings->NUM_TILES_XAXIS) &&
		(pos.y >= 0) && (pos.y < cSettings->NUM_TILES_YAXIS);
}

/**
 @brief Check if a grid is blocked
 */
bool CMap2D::isBlocked(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert) const
{
	if (bInvert == true)
	{
		if ((arrMapInfo[uiCurLevel][1][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value >= 100) &&
			(arrMapInfo[uiCurLevel][1][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value < 200))
			return true;
		else
			return false;
	}
	else
	{
		if ((arrMapInfo[uiCurLevel][1][uiRow][uiCol].value >= 100) &&
			(arrMapInfo[uiCurLevel][1][uiRow][uiCol].value < 200))
			return true;
		else
			return false;
	}
}

/**
 @brief Returns a 1D index based on a 2D coordinate using row-major layout
 */
int CMap2D::ConvertTo1D(const glm::vec2& pos) const
{
	//return (pos.y * m_dimensions.x) + pos.x;
	return (pos.y * cSettings->NUM_TILES_XAXIS) + pos.x;
}

/**
 @brief Delete AStar lists
 */
bool CMap2D::DeleteAStarLists(void)
{
	// Delete m_openList
	while (m_openList.size() != 0)
		m_openList.pop();
	// Delete m_cameFromList
	m_cameFromList.clear();
	// Delete m_closedList
	m_closedList.clear();

	return true;
}


/**
 @brief Reset AStar lists
 */
bool CMap2D::ResetAStarLists(void)
{
	// Delete m_openList
	while (m_openList.size() != 0)
		m_openList.pop();
	// Reset m_cameFromList
	for (int i = 0; i < m_cameFromList.size(); i++)
	{
		m_cameFromList[i].pos = glm::vec2(0, 0);
		m_cameFromList[i].parent = glm::vec2(0, 0);
		m_cameFromList[i].f = 0;
		m_cameFromList[i].g = 0;
		m_cameFromList[i].h = 0;
	}
	// Reset m_closedList
	for (int i = 0; i < m_closedList.size(); i++)
	{
		m_closedList[i] = false;
	}

	return true;
}


/**
 @brief manhattan calculation method for calculation of h, for games with no height
 */
unsigned int heuristic::manhattan(const glm::vec2& v1, const glm::vec2& v2, int weight)
{
	glm::vec2 delta = v2 - v1;
	return static_cast<unsigned int>(weight * (delta.x + delta.y));
}

/**
 @brief euclidean calculation method for calculation of h, for games with height
 */
unsigned int heuristic::euclidean(const glm::vec2& v1, const glm::vec2& v2, int weight)
{
	glm::vec2 delta = v2 - v1;
	return static_cast<unsigned int>(weight * sqrt((delta.x * delta.x) + (delta.y * delta.y)));
}
