// File: source\main.cpp
//
// Author: Ian Reynolds, 07/03/12

// include files
#include "application.h"

/// <summary>
///		Windows application entry point
///		Creates, initialises and runs the application object
/// </summary>
/// <param name="hInstance">		Windows application instance </param>
/// <param name="hPrevInstance">	Previous windows application instance </param>
/// <param name="lpCmdline">		Command line parameters for the application </param>
/// <param name="nCmdshow">			Controls how application window is to be shown </param>
/// <returns> WM_QUIT message exit code or zero if function terminates before entering the message loop </returns>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdline, int nCmdshow)
{
	// Declare the application instance
	Application* app = new Application;

	// Initialise the application
	if(!app->Init(hInstance))
	{
		MessageBox(NULL, L"Failed to initialise application", L"Application Error", NULL);
		return 0;
	}

	// Run the application, returning the WM_QUIT message exit code
	WPARAM exitCode = app->Run();

	// Exit the application
	if(!app->Exit())
	{
		MessageBox(NULL, L"Failed to exit application cleanly", L"Application Error", NULL);
	}

	// Release the application
	delete app;
	
	// Return the WM_QUIT message exit code to Windows
    return (int)exitCode;
}