////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "terrainclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "terrainshaderclass.h"
#include "lightclass.h"
#include "CubeClass.h"
#include "TextureShaderClass.h"
#include "RenderTextureClass.h"
#include "DebugWindowClass.h"
#include "OrthoWindowClass.h"
#include "VerticalBlurShaderClass.h"
#include "HorizontalBlurShaderClass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:
	bool HandleInput(float);
	bool RenderGraphics();
	bool RenderScene();
	bool RenderSceneToTexture();
	bool DownSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool UpSampleTexture();
	bool Render2DTextureScene();
	void ToggleBlur(bool);

private:
	InputClass* m_Input;
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	TerrainClass* m_Terrain;
	TimerClass* m_Timer;
	PositionClass* m_Position;
	FpsClass* m_Fps;
	CpuClass* m_Cpu;
	FontShaderClass* m_FontShader;
	TextClass* m_Text;
	TerrainShaderClass* m_TerrainShader;
	LightClass* m_Light;
	TextureClass* grassTexture;
	TextureClass* slopeTexture;
	TextureClass* rockTexture;
	CubeClass* cube;
	TextureShaderClass* textureShader;
	RenderTextureClass* m_RenderTexture;
	DebugWindowClass* m_debugWindow;

	HorizontalBlurShaderClass* m_horBlurShader;
	VerticalBlurShaderClass* m_verBlurShader;
	RenderTextureClass *m_RenderTex, *m_DownSampleTex, *m_HorBlurTex, *m_VerBlurTex, *m_UpSampleTex;
	OrthoWindowClass *m_SmallWindow, *m_FullScreenWindow;

	bool isBlur;
};

#endif