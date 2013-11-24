// File: source\d3d11window.cpp
//
// Author: Ian Reynolds, 07/03/12

// Include files
#include <Windows.h>

#include "d3d11renderer.h"

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#else
	dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}

/// <summary>
///		Default constructor
///		Initialises member variables to zero / null
///	</summary>
D3D11Renderer::D3D11Renderer()
	: screenWidth(0), screenHeight(0), device(NULL), immediateContext(NULL), swapChain(NULL), renderTargetView(NULL), depthStencilTexture(NULL), depthStencilView(NULL), solidRasterState(NULL), wireframeRasterState(NULL), renderWireframe(true)
{
}

/// <summary> Destructor </summary>
D3D11Renderer::~D3D11Renderer()
{
}

/// <summary> Initialises the device, context and effects </summary>
/// <param name="hWnd">	Handle of the application window </param>
/// <returns> true if it succeeds, false if it fails </returns>
bool D3D11Renderer::Init(HWND hWnd)
{
	// Initialise device, context and swap chain
	if(!InitDevice(hWnd))
	{
		MessageBox(NULL, L"Failed to initialise device", L"Renderer Error", NULL);
		return false;
	}
	
	// Create render target and depth stencil views
	if(!CreateRenderAndDepthViews())
	{
		MessageBox(NULL, L"Failed to create render target and depth stencil views", L"Renderer Error", NULL);
		return false;
	}

	// Setup the raster description which will determine how and what polygons will be drawn.
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	HRESULT hResult = device->CreateRasterizerState(&rasterDesc, &solidRasterState);
	if(FAILED(hResult))
	{
		return false;
	}

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hResult = device->CreateRasterizerState(&rasterDesc, &wireframeRasterState);
	if(FAILED(hResult))
	{
		return false;
	}

	// Now set the rasterizer state.
	if(renderWireframe)
	{
		immediateContext->RSSetState(wireframeRasterState);
	}
	else
	{
		immediateContext->RSSetState(solidRasterState);
	}

	InitEffects();

	InitRenderTextures();

	return true;
}

void D3D11Renderer::SetSolidFill()
{
	renderWireframe = false;
	immediateContext->RSSetState(solidRasterState);
}

void D3D11Renderer::SetWireframeFill()
{
	renderWireframe = true;
	immediateContext->RSSetState(wireframeRasterState);
}

/// <summary> Initialises the device, context and effects </summary>
/// <param name="hWnd">	Handle of the application window </param>
/// <returns> true if it succeeds, false if it fails </returns>
bool D3D11Renderer::InitDevice(HWND windowHandle)
{
	HRESULT hResult;
	RECT windowRect;

	// Get the window client area size
	GetClientRect(windowHandle, &windowRect);
	screenWidth = windowRect.right - windowRect.left;
	screenHeight = windowRect.bottom - windowRect.top;

	// Set the device flags
	unsigned int deviceFlags = 0;
	// TODO: figure out why the debug flag is causing a failure in creating device / swap chain
//#ifdef _DEBUG
//	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif
	
	// Set required driver types
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE
	};
	D3D_DRIVER_TYPE driverType = driverTypes[0];
	
	// Set required feature levels
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};
	unsigned int numFeatureLevels = 1;

	// Specify the swap chain descriptor
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = screenWidth;
    sd.BufferDesc.Height = screenHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = windowHandle;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

	// Create the device and swap chain
    hResult = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, deviceFlags, featureLevels, numFeatureLevels,
											D3D11_SDK_VERSION, &sd, &swapChain, &device, NULL, &immediateContext);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to create device and swap chain", L"Renderer Error", NULL);
		return false;
	}

    return true;
}

/// <summary> Creates the render target and depth stencil views </summary>
/// <returns> true if it succeeds, false if it fails </returns>
bool D3D11Renderer::CreateRenderAndDepthViews()
{
	HRESULT hResult;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hResult = swapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to get back buffer from swap chain", L"Renderer Error", NULL);
		return false;
	}
	
	hResult = device->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
	pBackBuffer->Release();
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to create render target view", L"Renderer Error", NULL);
		return false;
	}
	
	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = screenWidth;
	descDepth.Height = screenHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hResult = device->CreateTexture2D(&descDepth, NULL, &depthStencilTexture);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to create depth stencil texture", L"Renderer Error", NULL);
		return false;
	}
	
	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hResult = device->CreateDepthStencilView(depthStencilTexture, &descDSV, &depthStencilView);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to create depth stencil view", L"Renderer Error", NULL);
		return false;
	}
	
	// Set render target and depth stencil views in the context
	immediateContext->OMSetRenderTargets( 1, &renderTargetView, depthStencilView );

	return true;
}

/// <summary> Sets the viewport </summary>
void D3D11Renderer::SetViewport(D3D11_VIEWPORT viewport)
{
	// Setup the viewport
	immediateContext->RSSetViewports(1, &viewport);
}

/// <summary> Performs shutdown and cleanup functions for DirectX objects </summary>
/// <returns> true if it succeeds, false if it fails </returns>
bool D3D11Renderer::Exit()
{
	// Clear context state
	if(immediateContext)
	{
		immediateContext->ClearState();
	}

	// release shaders
	if(instancedGPUTerrainShader)
	{
		instancedGPUTerrainShader->Exit();
	}

	// Release depth stencil
    if(depthStencilTexture)
	{
		depthStencilTexture->Release();
	}

	// Release depth stencil view
    if(depthStencilView)
	{
		depthStencilView->Release();
	}

	// Release render target view
    if(renderTargetView)
	{
		renderTargetView->Release();
	}

	// release rasterization state
	if(solidRasterState)
	{
		solidRasterState->Release();
	}

	// release rasterization state
	if(wireframeRasterState)
	{
		wireframeRasterState->Release();
	}

	// Release swap chain
    if(swapChain)
	{
		swapChain->Release();
	}

	// Release context
    if(immediateContext)
	{
		immediateContext->Release();
	}

	// Release device
    if(device)
	{
		device->Release();
	}

	return true;
}

bool D3D11Renderer::InitEffects()
{
	lightingPassShader = new LightingPassShader();
	lightingPassShader->Init(device, immediateContext, L"fx/renderQuad.vs.hlsl", NULL, NULL, NULL, L"fx/terrainLighting.ps.hlsl");

	instancedGPUTerrainShader = new GPUInstDistContShader();
	instancedGPUTerrainShader->Init(device, immediateContext, L"fx/tessInstanced.vs.hlsl", L"fx/tess.hs.hlsl", L"fx/tess.ds.hlsl", NULL, L"fx/cont.ps.hlsl");

    return true;
}

bool D3D11Renderer::InitRenderTextures()
{
	positionBufferTexture = new RenderTexture(device, screenWidth, screenHeight);

	return true;
}

bool D3D11Renderer::Render()
{
	// Setup the color to clear the buffer to.
	float color[4];
	color[0] = 0.0f;
	color[1] = 0.125f;
	color[2] = 0.3f;
	color[3] = 1.0f;

	// Clear the back buffer.
	immediateContext->ClearRenderTargetView(renderTargetView, color);
    
	// Clear the depth buffer.
	immediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// render scene for each active camera in the camera list
	for(unsigned int i = 0; i < cameras.size(); ++i)
	{
		if(cameras[i]->IsActive())
		{
			// get the view and projection matrices
			XMFLOAT4X4 viewMatrix = cameras[i]->Render();
			XMFLOAT4X4 projectionMatrix = cameras[i]->GetProjectionMatrix();

			SetViewport(cameras[i]->GetViewport());

			if(!renderableMeshes.empty())
			{
				// Render the object list
				for(unsigned int j = 0; j < renderableMeshes.size(); ++j)
				{
					if(renderableMeshes[j]->IsActive())
					{
						// TODO: this is extremely inefficient, I should only be pushing each mesh once
						//renderableMeshes[j]->Render();
						//shader->Render(immediateContext, renderableMeshes[j]->GetIndexCount(), renderableMeshes[j]->transform->GetWorldMatrix(), viewMatrix, projectionMatrix);
					}
				}
			}
			
			for(unsigned int j = 0; j < renderableTerrains.size(); ++j)
			{
				if(renderableTerrains[j]->IsActive())
				{
					renderableTerrains[j]->Render(cameras[i], viewMatrix, projectionMatrix);
				}
			}
		}
	}

    // Present the back buffer
    swapChain->Present( 0, 0 );

	return true;
}

// NOTE: pass nullptr to set to default render target
void D3D11Renderer::SetRenderTargetView(ID3D11RenderTargetView* view)
{
	if(view)
	{
		immediateContext->OMSetRenderTargets(1, &view, depthStencilView);
	}
	else
	{
		immediateContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	}
}

void D3D11Renderer::ClearRenderLists()
{
	renderableMeshes.clear();
	renderableTerrains.clear();
}

void D3D11Renderer::AddRenderableMesh(Mesh* mesh)
{
	renderableMeshes.push_back(mesh);
}

void D3D11Renderer::AddRenderableTerrain(PlanetTerrain* terrain)
{
	renderableTerrains.push_back(terrain);
}

void D3D11Renderer::ClearCameraList()
{
	cameras.clear();
}

void D3D11Renderer::AddCamera(Camera* camera)
{
	cameras.push_back(camera);
}

ID3D11Device* D3D11Renderer::GetDevice()
{
	if(!device)
	{
		return NULL;
	}

	return device;
}

ID3D11DeviceContext* D3D11Renderer::GetDeviceContext()
{
	if(!immediateContext)
	{
		return NULL;
	}

	return immediateContext;
}

unsigned int D3D11Renderer::GetScreenWidth()
{
	return screenWidth;
}

unsigned int D3D11Renderer::GetScreenHeight()
{
	return screenHeight;
}

void D3D11Renderer::SetFullscreen()
{	
	swapChain->SetFullscreenState(true, nullptr);
}

void D3D11Renderer::SetWindowed()
{
	swapChain->SetFullscreenState(false, nullptr);
}