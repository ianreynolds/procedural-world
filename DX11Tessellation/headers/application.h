// File: headers\application.h
//
// Author: Ian Reynolds, 07/03/12

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

// Include files
#include <Windows.h>
#include <vector>

// Forward declarations
class ApplicationWindow;
class D3D11Renderer;
class Scene;

// Use the STD namespace for container objects
using namespace std;

/// <summary> Controls all aspects of the game application and contains all scene objects and managers </summary>
class Application
{
public:
	/// <summary>
	///		Default constructor
	///		Initialises all member variables to zero / null
	///	</summary>
	Application();

	/// <summary> Destructor </summary>
	~Application();

	/// <summary> Initialises application, scenes and managers </summary>
	/// <param name="hInstance"> Windows application instance </param>
	/// <returns> true if it succeeds, false if it fails </returns>
	bool Init(HINSTANCE hInstance);

	/// <summary> Controls update and render functions for the application managers </summary>
	/// <returns> WM_QUIT message exit code </returns>
	WPARAM Run();

	/// <summary> Exits application, shuts down managers and releases all resources </summary>
	/// <returns> true if it succeeds, false if it fails </returns>
	bool Exit();
private:

	/// <summary> Application window pointer </summary>
	ApplicationWindow* appWindow;

	/// <summary> Renderer pointer </summary>
	D3D11Renderer* renderer;

	/// <summary> STD vector containing the game engine scenes </summary>
	vector<Scene*> sceneList;
};

#endif