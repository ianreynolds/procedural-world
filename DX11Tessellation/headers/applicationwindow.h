// File: headers\applicationwindow.h
//
// Author: Ian Reynolds, 07/03/12

#ifndef _APPLICATIONWINDOW_H_
#define _APPLICATIONWINDOW_H_

// Include files
#include <Windows.h>

/// <summary> Controls creation of application window and contains window handle </summary>
class ApplicationWindow
{
public:
	/// <summary>
	///		Default constructor
	///		Initialises all member variables to zero / null
	///	</summary>
	ApplicationWindow();

	/// <summary> Destructor </summary>
	~ApplicationWindow();

	/// <summary> Static wrapper for window callback function </summary>
	/// <param name="hwnd">  	Window handle </param>
	/// <param name="msg">   	Message object </param>
	/// <param name="wParam">	wParam field of the message </param>
	/// <param name="lParam">	lParam field of the message </param>
	/// <returns> Result of message processing </returns>
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK StaticWndProcControl(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	LRESULT WndProcControl(UINT msg, WPARAM wParam, LPARAM lParam);

	/// <summary> Window callback function </summary>
	/// <param name="msg">   	Message object </param>
	/// <param name="wParam">	wParam field of the message </param>
	/// <param name="lParam">	lParam field of the message </param>
	/// <returns> Result of message processing </returns>
	LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

	/// <summary> Creates the application window </summary>
	/// <param name="hInstance"> Windows application instance </param>
	/// <returns> Handle of the newly created window </returns>
	HWND Init(HINSTANCE hInstance);

	/// <summary> Performs shutdown and cleanup functions for the application window </summary>
	/// <returns> true if it succeeds, false if it fails </returns>
	bool Exit();

	/// <summary> Accessor function for the application window handle </summary>
	/// <returns> The application window handle </returns>
	const HWND GetWindowHandle() const;
private:
	/// <summary> Handle of the application window </summary>
	HWND windowHandle;

	HWND childWindowHandle;
};

/// <summary> Accessor function for the application window handle </summary>
/// <returns> The application window handle </returns>
inline const HWND ApplicationWindow::GetWindowHandle() const
{
	return windowHandle;
}

#endif