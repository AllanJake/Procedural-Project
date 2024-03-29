////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "terrainclass.h"
#include <cmath>


TerrainClass::TerrainClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_heightMap = 0;
	m_terrainGeneratedToggle = false;
	m_terrainSmoothToggle = false;
	m_terrainResetToggle = false;
	m_grassTexture = 0;
	m_slopeTexture = 0;
	m_rockTexture = 0;
}


TerrainClass::TerrainClass(const TerrainClass& other)
{
}


TerrainClass::~TerrainClass()
{
}

bool TerrainClass::InitializeTerrain(ID3D11Device* device, int terrainWidth, int terrainHeight, WCHAR* grassTexture, WCHAR* slopeTexture, WCHAR* rockTexture)
{
	int index;
	float height = 0.0;
	bool result;
	PerlinSeed = rand() % 100;
	// Save the dimensions of the terrain.
	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;

	// Create the structure to hold the terrain data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if(!m_heightMap)
	{
		return false;
	}

	// Initialise the data in the height map (flat).
	for(int j=0; j<m_terrainHeight; j++)
	{
		for(int i=0; i<m_terrainWidth; i++)
		{			
			index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j;

		}
	}


	//even though we are generating a flat terrain, we still need to normalise it. 
	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if(!result)
	{
		return false;
	}

	// Calculate the texture coordinates.
	CalculateTextureCoords();

	// Load teh textures.
	result = LoadTexture(device, grassTexture, slopeTexture, rockTexture);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}
bool TerrainClass::Initialize(ID3D11Device* device, char* heightMapFilename)
{
	bool result;


	// Load in the height map for the terrain.
	result = LoadHeightMap(heightMapFilename);
	if(!result)
	{
		return false;
	}

	// Normalize the height of the height map.
	NormalizeHeightMap();

	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if(!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}


void TerrainClass::Shutdown()
{
	// Release the vertex and index buffer.
	ShutdownBuffers();

	// Release the height map data.
	ShutdownHeightMap();

	// Release the texture
	ReleaseTexture();

	return;
}

void TerrainClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int TerrainClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* TerrainClass::GetGrassTexture()
{
	return m_grassTexture->GetTexture();
}

ID3D11ShaderResourceView* TerrainClass::GetSlopeTexture()
{
	return m_slopeTexture->GetTexture();
}

ID3D11ShaderResourceView* TerrainClass::GetRockTexture()
{
	return m_rockTexture->GetTexture();
}

bool TerrainClass::ResetTerrain(ID3D11Device* device, bool keystroke)
{
	if (keystroke && (!m_terrainResetToggle))
	{
		int index;
		bool result;
		for (int j = 0; j<m_terrainHeight; j++)
		{
			for (int i = 0; i < m_terrainWidth; i++)
			{
					index = (m_terrainHeight * j) + i;

					m_heightMap[index].x = (float)i;
					m_heightMap[index].y = 0.0f;
					m_heightMap[index].z = (float)j;
			}
		}

		result = CalculateNormals();
		if (!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		m_terrainResetToggle = true;
	}
	else if (!keystroke && m_terrainResetToggle)
	{
		m_terrainResetToggle = false;
	}
	return true;
}

bool TerrainClass::GenerateTerrain(ID3D11Device* device, bool keystroke)
{
	if (keystroke)
	{
		int random = rand() % 30;
		
		if (random < 10)
		{
			GeneratePerlinTerrain(device, keystroke);
		}
		else if ((random >= 10) && (random < 20))
		{
			SmoothTerrain(device, keystroke);
		}
		else if ((random >= 20) && (random < 27))
		{
			DiamondSquareAlgorithm(device, keystroke, (rand() % 10), (rand() % 15), (rand() % 20));
		}
		else
		{
			FaultLine(device, keystroke);
		}
		return true;
	}
}

bool TerrainClass::FaultLine(ID3D11Device* device, bool keydown)
{
	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if (keydown && (!m_terrainGeneratedToggle))
	{
		int index;
		float height = 0.0;
		float peakHeight = RandomRange(-15, 15);
		float newHeight = peakHeight;
		int peakX = rand() & 86;
		int peakY = rand() & 48;
		float average;


		//loop through the terrain and set the hieghts how we want. This is where we generate the terrain
		//in this case I will run a sin-wave through the terrain in one axis.

		for (int j = peakY; j < m_terrainHeight; j++)
		{
			for (int i = peakX; i < m_terrainWidth; i++)
			{
				index = (m_terrainHeight * j) + i;

				/*m_heightMap[index].x = (float)i;
				m_heightMap[index].y = (float)newHeight;
				m_heightMap[index].z = (float)j;

				if (newHeight > 0.0)
					newHeight -= 1.0f;*/
				//////////////////////////////////////////////////////////////////////////////////////////////
				m_heightMap[index].x = (float)i;
				m_heightMap[index].y = (float)newHeight;
				m_heightMap[index].z = (float)j;
				average = 0.0f;
				//// Grab the average of the surrounding points
				// (-1, -1)
				//index = (m_terrainHeight * (j - 1) + (i - 1));
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}

				//// (0, -1)
				//index = (m_terrainHeight * (j - 1) + i);
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (1, -1)
				//index = (m_terrainHeight * (j - 1) + (i + 1));
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (-1, 0)
				//index = (m_terrainHeight * j + (i - 1));
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (0, 0)
				//index = (m_terrainHeight * j) + i;
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (1, 0)
				//index = (m_terrainHeight * j + (i + 1));
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (-1, 1)
				//index = (m_terrainHeight * (j + 1) + (i - 1));
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (0, 1)
				//index = (m_terrainHeight * (j + 1) + i);
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (1, 1)
				//index = (m_terrainHeight * (j + 1) + (i + 1));
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}



				//// (-2, -2)
				//index = (m_terrainHeight * (j - 2) + (i - 2));
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}

				//// (0, -2)
				//index = (m_terrainHeight * (j - 2) + i);
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (2, -2)
				//index = (m_terrainHeight * (j - 2) + (i + 2));
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (-2, 0)
				//index = (m_terrainHeight * j + (i - 2));
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (0, 0)
				//index = (m_terrainHeight * j) + i;
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (2, 0)
				//index = (m_terrainHeight * j + (i + 2));
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (-2, 2)
				//index = (m_terrainHeight * (j + 2) + (i - 2));
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (0, 2)
				//index = (m_terrainHeight * (j + 2) + i);
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}
				//// (2, 2)
				//index = (m_terrainHeight * (j + 2) + (i + 2));
				//if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				//{
				//	average += m_heightMap[index].y;
				//}

				//index = (m_terrainHeight * j) + i;

				//average = average / 18;

				//m_heightMap[index].x = (float)i;
				//m_heightMap[index].y += average;
				//m_heightMap[index].z = (float)j;
			}
				
		}
		//newHeight = peakHeight;
		

		result = CalculateNormals();
		if (!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		m_terrainGeneratedToggle = true;
	}
	else if (!keydown && (m_terrainGeneratedToggle))
	{
		m_terrainGeneratedToggle = false;
	}
	//SmoothTerrain(device, keydown);
	return true;
}

bool TerrainClass::GeneratePerlinTerrain(ID3D11Device* device, bool keydown)
{
	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if (keydown && (!m_terrainGeneratedToggle))
	{
		int index;
		//float height = 0.0;
		PerlinNoise Pn(PerlinSeed);


		//loop through the terrain and set the hieghts how we want. This is where we generate the terrain
		//in this case I will run a sin-wave through the terrain in one axis.

		for (int j = 0; j<m_terrainHeight; j++)
		{
			for (int i = 0; i < m_terrainWidth; i++)
			{
				if (i % 8 && j % 8) {
					index = (m_terrainHeight * j) + i;

					m_heightMap[index].x = (float)i;
					//m_heightMap[index].y = (float)(sin((float)i / (m_terrainWidth / 12))*8.0) + (cos((float)j / (m_terrainWidth / 15)) * 5.0f); //magic numbers ahoy, just to ramp up the height of the sin function so its visible.
					//m_heightMap[index].y = rand() % 9;
					m_heightMap[index].y += (Pn.noise(i, j, 1.5))*15;
					m_heightMap[index].z = (float)j;
				}
			}
		}

		result = CalculateNormals();
		if (!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		m_terrainGeneratedToggle = true;
	}
	else if (!keydown && (m_terrainGeneratedToggle))
	{
		m_terrainGeneratedToggle = false;
	}
	SmoothTerrain(device, keydown);
	return true;
}

bool TerrainClass::LoadHeightMap(char* filename)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k, index;
	unsigned char* bitmapImage;
	unsigned char height;


	// Open the height map file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Save the dimensions of the terrain.
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	imageSize = m_terrainWidth * m_terrainHeight * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Create the structure to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if(!m_heightMap)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k=0;

	// Read the image data into the height map.
	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			height = bitmapImage[k];
			
			index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j;

			k+=3;
		}
	}

	// Release the bitmap image data.
	delete [] bitmapImage;
	bitmapImage = 0;

	return true;
}

bool TerrainClass::SmoothTerrain(ID3D11Device* device, bool keydown)
{
	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if (keydown && (!m_terrainSmoothToggle))
	{
		int index;
		float average;
		//float height = 0.0;


		//loop through the terrain and set the hieghts how we want. This is where we generate the terrain
		//in this case I will run a sin-wave through the terrain in one axis.

		for (int j = 0; j < m_terrainHeight; j++)
		{
			for (int i = 0; i < m_terrainWidth; i++)
			{
				average = 0.0f;
				//// Grab the average of the surrounding points
				// (-1, -1)
				index = (m_terrainHeight * (j - 1) + (i - 1));
				if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				{
					average += m_heightMap[index].y;
				}

				// (0, -1)
				index = (m_terrainHeight * (j - 1) + i);
				if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				{
					average += m_heightMap[index].y;
				}
				// (1, -1)
				index = (m_terrainHeight * (j - 1) + (i + 1));
				if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				{
					average += m_heightMap[index].y;
				}
				// (-1, 0)
				index = (m_terrainHeight * j + (i - 1));
				if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				{
					average += m_heightMap[index].y;
				}
				// (0, 0)
				index = (m_terrainHeight * j) + i;
				if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				{
					average += m_heightMap[index].y;
				}
				// (1, 0)
				index = (m_terrainHeight * j + (i + 1));
				if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				{
					average += m_heightMap[index].y;
				}
				// (-1, 1)
				index = (m_terrainHeight * (j + 1) + (i - 1));
				if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				{
					average += m_heightMap[index].y;
				}
				// (0, 1)
				index = (m_terrainHeight * (j + 1) + i);
				if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				{
					average += m_heightMap[index].y;
				}
				// (1, 1)
				index = (m_terrainHeight * (j + 1) + (i + 1));
				if ((index < m_terrainHeight * m_terrainWidth) && (index > 0))
				{
					average += m_heightMap[index].y;
				}

				average = average / 9;

				index = (m_terrainHeight * j) + i;

				m_heightMap[index].x = (float)i;
				m_heightMap[index].y = average;
				m_heightMap[index].z = (float)j;
			}
		}

		result = CalculateNormals();
		if (!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		m_terrainSmoothToggle = true;
	}

	else
	{
		m_terrainSmoothToggle = false;
	}

	return true;
}

void TerrainClass::NormalizeHeightMap()
{
	int i, j;


	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			m_heightMap[(m_terrainHeight * j) + i].y /= 15.0f;
		}
	}

	return;
}


bool TerrainClass::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new VectorType[(m_terrainHeight-1) * (m_terrainWidth-1)];
	if(!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for(j=0; j<(m_terrainHeight-1); j++)
	{
		for(i=0; i<(m_terrainWidth-1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i+1);
			index3 = ((j+1) * m_terrainHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;
		
			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;
		
			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight-1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if(((i-1) >= 0) && ((j-1) >= 0))
			{
				index = ((j-1) * (m_terrainHeight-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if((i < (m_terrainWidth-1)) && ((j-1) >= 0))
			{
				index = ((j-1) * (m_terrainHeight-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if(((i-1) >= 0) && (j < (m_terrainHeight-1)))
			{
				index = (j * (m_terrainHeight-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if((i < (m_terrainWidth-1)) && (j < (m_terrainHeight-1)))
			{
				index = (j * (m_terrainHeight-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}
			
			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));
			
			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete [] normals;
	normals = 0;

	return true;
}


void TerrainClass::ShutdownHeightMap()
{
	if(m_heightMap)
	{
		delete [] m_heightMap;
		m_heightMap = 0;
	}

	return;
}

void TerrainClass::CalculateTextureCoords()
{
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;


	// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)TEXTURE_REPEAT / (float)m_terrainWidth;

	// Calculate how many times to repeat the texture.
	incrementCount = m_terrainWidth / TEXTURE_REPEAT;

	// Initialize the tu and tv coordinate values.
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	tuCount = 0;
	tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			// Store the texture coordinate in the height map.
			m_heightMap[(m_terrainHeight * j) + i].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * j) + i].tv = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}

	return;
}

bool TerrainClass::LoadTexture(ID3D11Device* device, WCHAR* grassTexture, WCHAR* slopeTexture, WCHAR* rockTexture)
{
	bool result;

	//create the grass texture object
	m_grassTexture = new TextureClass;
	if (!m_grassTexture)
		return false;

	// initialise the grass tex object
	result = m_grassTexture->Initialize(device, grassTexture);
	if (!result)
		return false;

	//create the slope texture object
	m_slopeTexture = new TextureClass;
	if (!m_slopeTexture)
		return false;

	// initialise the slope tex object
	result = m_slopeTexture->Initialize(device, slopeTexture);
	if (!result)
		return false;

	//create the rock texture object
	m_rockTexture = new TextureClass;
	if (!m_rockTexture)
		return false;

	// initialise the grass tex object
	result = m_rockTexture->Initialize(device, rockTexture);
	if (!result)
		return false;

	return true;
}

void TerrainClass::ReleaseTexture()
{
	// Release the texture objects.
	if (m_grassTexture)
	{
		m_grassTexture->Shutdown();
		delete m_grassTexture;
		m_grassTexture = 0;
	}
	if (m_slopeTexture)
	{
		m_slopeTexture->Shutdown();
		delete m_slopeTexture;
		m_slopeTexture = 0;
	}
	if (m_rockTexture)
	{
		m_rockTexture->Shutdown();
		delete m_rockTexture;
		m_rockTexture = 0;
	}
	
	return;
}


bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int index1, index2, index3, index4;
	float tu, tv;


	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;

	// Load the vertex and index array with the terrain data.
	for(j=0; j<(m_terrainHeight-1); j++)
	{
		for(i=0; i<(m_terrainWidth-1); i++)
		{
			index1 = (m_terrainHeight * j) + i;          // Bottom left.
			index2 = (m_terrainHeight * j) + (i+1);      // Bottom right.
			index3 = (m_terrainHeight * (j+1)) + i;      // Upper left.
			index4 = (m_terrainHeight * (j+1)) + (i+1);  // Upper right.

			// Upper Left.
			tv = m_heightMap[index3].tv;

			// Modify the texture coords to cover the top edge.
			if (tv == 1.0f)
				tv = 0.0f;

			vertices[index].position = D3DXVECTOR3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			vertices[index].texture = D3DXVECTOR2(m_heightMap[index3].tu, tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			indices[index] = index;
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coords to cover top and right edge
			if (tu == 0.0f)		tu = 1.0f;
			if (tv == 1.0f)		tv = 0.0f;

			vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			vertices[index].texture = D3DXVECTOR2(tu, tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			vertices[index].texture = D3DXVECTOR2(m_heightMap[index1].tu, m_heightMap[index1].tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			vertices[index].texture = D3DXVECTOR2(m_heightMap[index1].tu, m_heightMap[index1].tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Upper right.
			vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			vertices[index].texture = D3DXVECTOR2(tu, tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom right.
			tu = m_heightMap[index2].tu;

			//modify the tex coords to cover the right edge
			if (tu == 0.0f)		tu = 1.0f;

			vertices[index].position = D3DXVECTOR3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			vertices[index].texture = D3DXVECTOR2(tu, m_heightMap[index2].tv);
			vertices[index].normal = D3DXVECTOR3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			indices[index] = index;
			index++;
		}
	}

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void TerrainClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void TerrainClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

float TerrainClass::RandomRange(float min, float max)
{
	if (max < min)
	{
		return 0.0f;
	}
	return (((rand() % 20000) / 20000.0f) * (max - min) + min);
}

bool TerrainClass::DiamondSquareAlgorithm(ID3D11Device* device, bool keystroke, float cornerHeight, float randRange, float heightScalar)
{
	bool result;
	std::vector<float> heights;
	int index, numOfIterations, step;

	// Bool is used to call the function only once
	if (keystroke && !m_terrainGeneratedToggle)
	{


		// Initialize variables
		step = (m_terrainHeight - 1);
		index = numOfIterations = 0;
		heights.resize(m_terrainHeight * m_terrainWidth);

		for (int i = 0; i < (int)heights.size(); i++)
		{
			heights[i] = 0.0f; // Initialize all the heights to 0 for flat terrain
		}

		// Set the corner heights
		heights[0] = cornerHeight;		// Bottom Left
		heights[(m_terrainHeight * (m_terrainHeight - 1))] = cornerHeight;		// Top Left
		heights[(m_terrainWidth - 1)] = cornerHeight;		// Bottom Right
		heights[(m_terrainHeight * (m_terrainHeight - 1)) + (m_terrainWidth - 1)] = cornerHeight;		// Top Right

		while (step > 1)
		{
			// Diamond Step
			numOfIterations++;
			step /= 2;
			index = 0;

			// Loop through Center points
			for (int j = step; j < m_terrainHeight - step; j += (step * 2))
			{
				for (int i = step; i < m_terrainWidth - step; i += (step * 2))
				{
					// Diamond Step
					//Init the height
					float averageHeight = 0.0f;

					// Top Left
					averageHeight += heights[(m_terrainHeight * (j - step)) + (i - step)];
					// Top Right
					averageHeight += heights[(m_terrainHeight * (j - step)) + (i + step)];
					// Bottom Left
					averageHeight += heights[(m_terrainHeight * (j + step)) + (i - step)];
					// Bottom Right
					averageHeight += heights[(m_terrainHeight * (j + step)) + (i + step)];

					// Get current index
					index = (m_terrainHeight * j) + i;

					float smoothingValue = (float)numOfIterations;

					// set average of four corners
					heights[index] = (averageHeight / 4) +RandomRange(randRange-5, randRange) / smoothingValue;

					// Square Step
					// Top
					if ((j - step) >= 0)
					{
						heights[(m_terrainHeight * (j - step)) + i] = GetSquareAverage(heights, i, (j - step), step, randRange, smoothingValue);
					}
					// Right
					if ((i + step) >= 0)
					{
						heights[(m_terrainHeight * j) + (i + step)] = GetSquareAverage(heights, (i + step), j, step, randRange, smoothingValue);
					}
					// Bottom
					if ((j + step) >= 0)
					{
						heights[(m_terrainHeight * (j + step)) + i] = GetSquareAverage(heights, i, (j + step), step, randRange, smoothingValue);
					}
					// Left
					if ((i - step) >= 0)
					{
						heights[(m_terrainHeight * j) + (i - step)] = GetSquareAverage(heights, (i - step), j, step, randRange, smoothingValue);
					}
				}
			}
		}
		for (int j = 0; j < m_terrainHeight; j++)
		{
			for (int i = 0; i < m_terrainWidth; i++)
			{
				index = (m_terrainHeight * j) + i;
				m_heightMap[index].x = float(i);
				m_heightMap[index].y += (heights[index])*2;// +(cornerHeight / 2)) * heightScalar;
				m_heightMap[index].z = float(j);
			}
		}

		result = CalculateNormals();
		if (!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}
		m_terrainGeneratedToggle = true;
	}
	else if (!keystroke && m_terrainGeneratedToggle)
	{
		m_terrainGeneratedToggle = false;
	}
	heights.clear();
	return true;
}

float TerrainClass::GetSquareAverage(std::vector<float> &vector, int i, int j, int step, float randRange, float smoothingVal)
{
	// Init variables
	float averageHeight = 0.0f;
	int numOfAverages = 0;

	// Top
	if ((j - step) >= 0)
	{
		averageHeight += vector[(m_terrainHeight * (j - step) + i)];
		numOfAverages++;
	}
	// Right
	if ((i + step) < m_terrainWidth)
	{
		averageHeight += vector[(m_terrainHeight * j + (i + step))];
		numOfAverages++;
	}
	// Bottom
	if ((j + step) < m_terrainHeight)
	{
		averageHeight += vector[(m_terrainHeight * (j + step) + i)];
		numOfAverages++;
	}
	// Left
	if ((i - step) >= 0)
	{
		averageHeight += vector[(m_terrainHeight * j + (i - step))];
		numOfAverages++;
	}

	float newHeight = (averageHeight / numOfAverages) + RandomRange(-randRange, randRange) / smoothingVal;

	return newHeight;

}