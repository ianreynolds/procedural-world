// File: source\application.cpp
//
// Author: Ian Reynolds, 07/03/12

// Include files
#include "application.h"
#include "applicationwindow.h"
#include "d3d11renderer.h"
#include "scene.h"
#include "di8inputmanager.h"

#include "metrics.h"

/// <summary>
///		Default constructor
///		Initialises all member variables to zero / null
///	</summary>
Application::Application()
	: appWindow(NULL), renderer(NULL)
{
}

/// <summary> Destructor </summary>
Application::~Application()
{
}

/// <summary> Initialises application, scenes and managers  </summary>
/// <param name="hInstance"> Windows application instance. </param>
/// <returns> true if it succeeds, false if it fails </returns>
bool Application::Init(HINSTANCE hInstance)
{
	// Initialise window
	appWindow = new ApplicationWindow();
	HWND windowHandle = appWindow->Init(hInstance);
	if(windowHandle == NULL)
	{
		MessageBox(NULL, L"Failed to initialise window", L"Window Error", NULL);
		return false;
	}

	// Initialise renderer & effects
	renderer = new D3D11Renderer();
	if(!renderer->Init(windowHandle))
	{
		MessageBox(NULL, L"Failed to initialise renderer", L"Renderer Error", NULL);
		return false;
	}

	// Initialise input manager
	if(!DI8InputManager::GetInstance()->Init(windowHandle, hInstance))
	{
		MessageBox(NULL, L"Failed to initialise input manager", L"Input Error", NULL);
		return false;
	}

	// Initialise scenes
	Scene* planetTerrainScene = new Scene(renderer);
	sceneList.push_back(planetTerrainScene);

	Scene::SetCurrentScene(planetTerrainScene);

	GameObject* terrainObject = new GameObject();

	PlanetTerrain* planetTerrainComponent = new PlanetTerrain(renderer->GetDevice(), renderer->GetDeviceContext(), renderer,
		renderer->GetInstancedGPUTerrainShader(), renderer->GetLightingPassShader(), renderer->GetPositionBufferTexture());

	PlanetTerrainController* planetTerrainController = new PlanetTerrainController(planetTerrainComponent);

	terrainObject->AddComponent(ComponentEnums::Terrain, planetTerrainComponent);
	terrainObject->AddComponent(ComponentEnums::Controller, planetTerrainController);
	
	GameObject* renderControllerObject = new GameObject();

	RenderController* renderController = new RenderController(renderer);

	renderControllerObject->AddComponent(ComponentEnums::Controller, renderController);

	GameObject* planarCameraObject = new GameObject();
	planarCameraObject->transform->SetLookAt(XMFLOAT3(0.0f, 18000.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));

	D3D11_VIEWPORT viewport;
	viewport.Width = (float)renderer->GetScreenWidth();
	viewport.Height = (float)renderer->GetScreenHeight();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	FlyCameraController* flyCameraController = new FlyCameraController(500.0f, 0.6f);
	Camera* planarCamera = new Camera(viewport, XM_PIDIV4, viewport.Width / viewport.Height, 0.001f, 50000.0f);

	planarCameraObject->AddComponent(ComponentEnums::Controller, flyCameraController);
	planarCameraObject->AddComponent(ComponentEnums::Camera, planarCamera);

	GameObject* topDownCameraObject = new GameObject();
	topDownCameraObject->transform->SetLookAt(XMFLOAT3(0.0f, 40.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));

	viewport.Width = (float)renderer->GetScreenHeight() / 4.0f;
	viewport.Height = (float)renderer->GetScreenHeight() / 4.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	Camera* topDownCamera = new Camera(viewport, XM_PIDIV4, viewport.Width / viewport.Height, 0.01f, 100.0f);
	topDownCamera->Deactivate();

	topDownCameraObject->AddComponent(ComponentEnums::Camera, topDownCamera);

	Scene::GetCurrentScene()->StartScene();

	return true;
}

/// <summary> Controls update and render functions for the application managers </summary>
/// <returns> WM_QUIT message exit code </returns>
WPARAM Application::Run()
{
	// Windows message loop, updates and renders during idle time
    MSG msg = {0};

	// Declare last time and take a current reading
	LARGE_INTEGER lastTime, currentTime;
	QueryPerformanceCounter(&lastTime);

	// Get the high performance counter frequency
	LARGE_INTEGER timerFrequency;
	QueryPerformanceFrequency(&timerFrequency);

    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
			// Update delta time
			QueryPerformanceCounter(&currentTime);
			double deltaTime = (double)(currentTime.QuadPart - lastTime.QuadPart) / (double)(timerFrequency.QuadPart);
			lastTime = currentTime;

			double fps = 1.0 / deltaTime;

			char displayString[199];
			_snprintf_s(displayString, 199, "Application Window    FPS: %f    Frame Time: %f    Altitude: %f    LOD: %d    Quads: %d", fps, (float)deltaTime * 1000.0f, Metrics::cameraAltitude, Metrics::highestLOD, Metrics::quadsInstances);

			SetWindowTextA(appWindow->GetWindowHandle(), (LPCSTR)displayString);
			//OutputDebugStringA(displayString);

			// Update the input manager
			DI8InputManager::GetInstance()->Update();

			// Update the current scene
			Scene::GetCurrentScene()->UpdateScene(deltaTime);

			// TODO: physics step...
			
			// Post update the current scene
			Scene::GetCurrentScene()->PostUpdateScene(deltaTime);

			// Render the scene
			// TODO: link this to the scene???
			renderer->Render();
        }
    }

	// Return the WM_QUIT message exit code
	return msg.wParam;
}

/// <summary> Exits application, shuts down managers and releases all resources </summary>
/// <returns> true if it succeeds, false if it fails </returns>
bool Application::Exit()
{
	// Shutdown and release application window
	if(appWindow)
	{
		if(!appWindow->Exit())
		{
			MessageBox(NULL, L"Failed to shutdown window", L"Window Error", NULL);
		}
		delete appWindow;
	}
	
	// Shutdown and release renderer
	if(renderer)
	{
		if(!renderer->Exit())
		{
			MessageBox(NULL, L"Failed to shutdown renderer", L"Renderer Error", NULL);
		}
		delete renderer;
	}

	// Release all scenes
	for(unsigned int i = 0; i < sceneList.size(); ++i)
	{
		delete sceneList[i];
	}

	return true;
}