#ifndef RENDERTEXTURECLASS_H
#define RENDERTEXTURECLASS_H

#include <D3D11.h>
#include <D3DX10math.h>

class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D11Device*, int, int);
	bool Initialize(ID3D11Device*, int, int, float, float);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();

	void GetOrthoMatrix(D3DXMATRIX&);
	int GetTextureWidth();
	int GetTextureHeight();

private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	D3DXMATRIX m_orthoMatrix;
	int texWidth, texHeight;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11Texture2D* m_depthStencilBuffer;
};

#endif // !RENDERTEXTURECLASS_H