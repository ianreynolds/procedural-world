// File: headers\d3d11window.h
//
// Author: Ian Reynolds, 07/03/12

#ifndef _D3D11RENDERER_H_
#define _D3D11RENDERER_H_

// Include files
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <vector>

#include "GPUInstDistContShader.h"
#include "LightingPassShader.h"
#include "RenderTexture.h"
#include "gameobject.h"

// Use the STD namespace for container objects
using namespace std;

/// <summary>
///		DirectX11 renderer class
///		Controls initialisation of DirectX device, context and effects
///		Controls rendering of renderable objects
/// </summary>
class D3D11Renderer
{
public:
	/// <summary>
	///		Default constructor
	///		Initialises member variables to zero / null
	///	</summary>
	D3D11Renderer();

	/// <summary> Destructor </summary>
	~D3D11Renderer();

	/// <summary> Initialises the device, context and effects </summary>
	/// <param name="hWnd">	Handle of the application window </param>
	/// <returns> true if it succeeds, false if it fails </returns>
	bool Init(HWND hWnd);

	/// <summary> Performs shutdown and cleanup functions for DirectX objects </summary>
	/// <returns> true if it succeeds, false if it fails </returns>
	bool Exit();

	/// <summary> clears the render lists ready to be used by a new scene </summary>
	void ClearRenderLists();

	/// <summary> adds renderable mesh to the render list </summary>
	/// <param name="mesh"> mesh pointer to be added </param>
	void AddRenderableMesh(Mesh* mesh);

	/// <summary> adds renderable terrain to the render list </summary>
	/// <param name="terrain"> mesh pointer to be added </param>
	void AddRenderableTerrain(PlanetTerrain* terrain);

	/// <summary> clears the camera list ready to be used by a new scene </summary>
	void ClearCameraList();

	/// <summary> adds camera to the camera list </summary>
	/// <param name="mesh"> camera pointer to be added </param>
	void AddCamera(Camera* camera);

	/// <summary> gets a pointer to the DirectX11 device </summary>
	/// <returns> device pointer or null if device does not exist </returns>
	ID3D11Device* GetDevice();

	/// <summary> gets a pointer to the DirectX11 device context. </summary>
	/// <returns> device context pointer or null if device context does not exist </returns>
	ID3D11DeviceContext* GetDeviceContext();

	/// <summary> gets the screen width </summary>
	/// <returns> screen width </returns>
	unsigned int GetScreenWidth();

	/// <summary> gets the screen height </summary>
	/// <returns> screen height </returns>
	unsigned int GetScreenHeight();

	void SetRenderTargetView(ID3D11RenderTargetView* view);

	bool InitEffects();

	// TODO: some renaming needed here, don't need GPU or instanced descriptors, lighting should be specific for terrain
	Shader* instancedGPUTerrainShader;
	Shader* lightingPassShader;

	Shader* GetInstancedGPUTerrainShader() { return instancedGPUTerrainShader; }
	Shader* GetLightingPassShader() { return lightingPassShader; }

	bool InitRenderTextures();

	RenderTexture* positionBufferTexture;

	RenderTexture* GetPositionBufferTexture() { return positionBufferTexture; }

	bool Render();

	void SetSolidFill();
	void SetWireframeFill();
	void SetFullscreen();
	void SetWindowed();

	bool renderWireframe;

	bool IsWireframe() { return renderWireframe; }
private:
	/// <summary> Initialises the device and context</summary>
	/// <param name="hWnd">	Handle of the application window </param>
	/// <returns> true if it succeeds, false if it fails </returns>
	bool InitDevice(HWND hWnd);

	/// <summary> Creates the render target and depth stencil views </summary>
	/// <returns> true if it succeeds, false if it fails </returns>
	bool CreateRenderAndDepthViews();

	/// <summary> Sets the viewport </summary>
	void SetViewport(D3D11_VIEWPORT viewport);

	/// <summary> Width of the screen </summary>
	unsigned int screenWidth;

	/// <summary> Height of the screen </summary>
	unsigned int screenHeight;

	/// <summary> DirectX device pointer </summary>
	ID3D11Device* device;

	/// <summary> Immediate context pointer </summary>
	ID3D11DeviceContext* immediateContext;

	/// <summary> DirectX swap chain pointer </summary>
	IDXGISwapChain* swapChain;

	/// <summary> Render target view pointer </summary>
	ID3D11RenderTargetView* renderTargetView;

	/// <summary> Depth stencil texture pointer </summary>
	ID3D11Texture2D* depthStencilTexture;

	/// <summary> Depth stencil view pointer </summary>
	ID3D11DepthStencilView* depthStencilView;

	/// <summary> Solid fill rasterizer state </summary>
	ID3D11RasterizerState* solidRasterState;

	/// <summary> Wireframe fill rasterizer state </summary>
	ID3D11RasterizerState* wireframeRasterState;

	/// <summary> STD vector containing the game engine renderable meshes </summary>
	vector<Mesh*> renderableMeshes;

	/// <summary> STD vector containing the game engine renderable terrains </summary>
	vector<PlanetTerrain*> renderableTerrains;

	/// <summary> STD vector containing the game engine cameras </summary>
	vector<Camera*> cameras;
};

#endif