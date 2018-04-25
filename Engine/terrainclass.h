////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdio.h>
#include "PerlinNoise.h"
#include "textureclass.h"

// GLOBALS
const int TEXTURE_REPEAT = 32;

////////////////////////////////////////////////////////////////////////////////
// Class name: TerrainClass
////////////////////////////////////////////////////////////////////////////////

class TerrainClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	    D3DXVECTOR3 normal;
	};

	struct HeightMapType 
	{ 
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType 
	{ 
		float x, y, z;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(ID3D11Device*, char*);
	bool InitializeTerrain(ID3D11Device*, int terrainWidth, int terrainHeigt, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	bool GenerateWater(ID3D11Device* device, float frameTime);
	bool GeneratePerlinTerrain(ID3D11Device* device, bool keystroke);
	bool SmoothTerrain(ID3D11Device* device, bool keydown);
	int  GetIndexCount();

	ID3D11ShaderResourceView* GetGrassTexture();
	ID3D11ShaderResourceView* GetSlopeTexture();
	ID3D11ShaderResourceView* GetRockTexture();


private:
	bool LoadHeightMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	void CalculateTextureCoords();
	bool LoadTexture(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
	void ReleaseTexture();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	
private:
	bool m_terrainGeneratedToggle;
	bool m_terrainSmoothToggle;
	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount, m_indexCount;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	HeightMapType* m_heightMap;
	TextureClass *m_grassTexture, *m_slopeTexture, *m_rockTexture;
	
};

#endif