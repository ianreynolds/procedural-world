#include "RenderTexture.h"

RenderTexture::RenderTexture(ID3D11Device* device, float width, float height)
	: renderTargetTexture(nullptr), renderTargetView(nullptr), shaderResourceView(nullptr)
{
	HRESULT hResult;

	D3D11_TEXTURE2D_DESC renderTargetTextureDesc;

	renderTargetTextureDesc.ArraySize = 1;
	renderTargetTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTargetTextureDesc.CPUAccessFlags = 0;
	renderTargetTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTargetTextureDesc.Width = width;
	renderTargetTextureDesc.Height = height;
	renderTargetTextureDesc.MipLevels = 1;
	renderTargetTextureDesc.MiscFlags = 0;
	renderTargetTextureDesc.SampleDesc.Count = 1;
	renderTargetTextureDesc.SampleDesc.Quality = 0;
	renderTargetTextureDesc.Usage = D3D11_USAGE_DEFAULT;

	hResult = device->CreateTexture2D(&renderTargetTextureDesc, nullptr, &renderTargetTexture);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to create render target texture", L"Render Texture Error", MB_OK);
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

	renderTargetViewDesc.Format = renderTargetTextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	hResult = device->CreateRenderTargetView(renderTargetTexture, &renderTargetViewDesc, &renderTargetView);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to create render target view", L"Render Texture Error", MB_OK);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = renderTargetTextureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	hResult = device->CreateShaderResourceView(renderTargetTexture, &shaderResourceViewDesc, &shaderResourceView);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to create shader resource view", L"Render Texture Error", MB_OK);
	}
}