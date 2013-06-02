#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include "D3D11.h"

class RenderTexture
{
public:
	RenderTexture(ID3D11Device* device, float width, float height);

	ID3D11RenderTargetView* GetRenderTargetView() { return renderTargetView; }
	ID3D11ShaderResourceView* GetShaderResourceView() { return shaderResourceView; }

protected:
	ID3D11Texture2D* renderTargetTexture;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11ShaderResourceView* shaderResourceView;
};

#endif