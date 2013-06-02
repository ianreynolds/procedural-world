// File: source\applicationwindow.cpp
//
// Author: Ian Reynolds, 07/03/12

// Include files
#include "applicationwindow.h"

#define IDC_EXIT_BUTTON		101
#define IDC_LOD_COMBO		102
#define IDC_MAX_DIST_EDIT	103

/// <summary>
///		Default constructor
///		Initialises all member variables to zero / null
///	</summary>
ApplicationWindow::ApplicationWindow()
	: windowHandle(NULL)
{
}

/// <summary> Destructor </summary>
ApplicationWindow::~ApplicationWindow()
{
}

/// <summary> Static wrapper for window callback function </summary>
/// <param name="hwnd">  	Window handle </param>
/// <param name="msg">   	Message object </param>
/// <param name="wParam">	wParam field of the message </param>
/// <param name="lParam">	lParam field of the message </param>
/// <returns> Result of message processing </returns>
LRESULT CALLBACK ApplicationWindow::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ApplicationWindow* appWindow;
	
	// Get pointer to window
	if(msg == WM_CREATE)
	{
		appWindow = (ApplicationWindow*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)appWindow);
	}
	else
	{
		appWindow = (ApplicationWindow*)GetWindowLongPtr(hWnd, GWL_USERDATA);
		if(!appWindow)
		{
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}

	return appWindow->WndProc(msg,wParam,lParam);
}

LRESULT CALLBACK ApplicationWindow::StaticWndProcControl(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ApplicationWindow* appWindow;
	
	// Get pointer to window
	if(msg == WM_CREATE)
	{
		appWindow = (ApplicationWindow*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)appWindow);
	}
	else
	{
		appWindow = (ApplicationWindow*)GetWindowLongPtr(hWnd, GWL_USERDATA);
		if(!appWindow)
		{
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}

	return appWindow->WndProcControl(msg,wParam,lParam);
}

LRESULT ApplicationWindow::WndProcControl(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDC_EXIT_BUTTON:
				{
					SendMessage(windowHandle, WM_DESTROY, NULL, NULL);
				}
				break;
			case IDC_LOD_COMBO:
				{
					if(HIWORD(wParam) == CBN_SELCHANGE)
					{
						MessageBox(NULL, L"HI", NULL, NULL);
					}
				}
				break;
			}
		}
		break;
	}

	return DefWindowProc(childWindowHandle, msg, wParam, lParam);
}

/// <summary> Window callback function </summary>
/// <param name="msg">   	Message object </param>
/// <param name="wParam">	wParam field of the message </param>
/// <param name="lParam">	lParam field of the message </param>
/// <returns> Result of message processing </returns>
LRESULT ApplicationWindow::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Process message
	switch(msg)
	{
	case WM_DESTROY:
		// Post quit message to windows
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		// exit application on espape key
		if( wParam == VK_ESCAPE )
		{
			DestroyWindow(windowHandle);
		}
		break;
	}

	return DefWindowProc(windowHandle, msg, wParam, lParam);
}

/// <summary> Creates the application window </summary>
/// <param name="hInstance"> Windows application instance </param>
/// <returns> Handle of the newly created window </returns>
HWND ApplicationWindow::Init(HINSTANCE hInstance)
{
	// Create window class
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ApplicationWindow::StaticWndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"ApplicationWindowClass";

	// Register window class
    if(!RegisterClass(&wc))
	{
		MessageBox(NULL, L"Failed to register windows class", L"Window Error", NULL);
        return NULL;
	}

    // Create window, 
    RECT windowRect = { 0, 0, 800, 600 };
    windowHandle = CreateWindow(L"ApplicationWindowClass", L"Application Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
								windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInstance, this);

    if(!windowHandle)
	{
		MessageBox(NULL, L"Failed to create window", L"Window Error", NULL);
        return NULL;
	}

	// Show the window
    ShowWindow(windowHandle, SW_SHOW);

	// Create window class
    WNDCLASS wcChild;
    wcChild.style = CS_HREDRAW | CS_VREDRAW;
    wcChild.lpfnWndProc = ApplicationWindow::StaticWndProcControl;
    wcChild.cbClsExtra = NULL;
    wcChild.cbWndExtra = NULL;
    wcChild.hInstance = hInstance;
    wcChild.hIcon = LoadIcon(0, IDI_APPLICATION);
    wcChild.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcChild.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcChild.lpszMenuName = NULL;
    wcChild.lpszClassName = L"ApplicationChildWindowClass";

	// Register window class
    if(!RegisterClass(&wcChild))
	{
		MessageBox(NULL, L"Failed to register windows class", L"Window Error", NULL);
        return NULL;
	}

    // Create window, 
 //   RECT windowRectChild = { 0, 0, 400, 800 };
 //   childWindowHandle = CreateWindow(L"ApplicationChildWindowClass", L"Control Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
	//							windowRectChild.right - windowRectChild.left, windowRectChild.bottom - windowRectChild.top, NULL, NULL, hInstance, this);

 //   if(!childWindowHandle)
	//{
	//	MessageBox(NULL, L"Failed to create window", L"Window Error", NULL);
 //       return NULL;
	//}

	//HGDIOBJ hfDefault=GetStockObject(DEFAULT_GUI_FONT);

	//// Create exit button
	//HWND hWndExitButton=CreateWindow( 
	//	L"BUTTON",
	//	L"Exit",
	//	WS_TABSTOP|WS_VISIBLE|
	//	WS_CHILD|BS_DEFPUSHBUTTON,
	//	50,
	//	600,
	//	75,
	//	23,
	//	childWindowHandle,
	//	(HMENU)IDC_EXIT_BUTTON,
	//	GetModuleHandle(NULL),
	//	NULL);

	//SendMessage(hWndExitButton,
	//	WM_SETFONT,
	//	(WPARAM)hfDefault,
	//	MAKELPARAM(FALSE,0));

	//// Create a LOD dropdown
	//HWND hWndLodDropDown=CreateWindow( 
	//	L"COMBOBOX",
	//	L"",
	//	CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
	//	50,
	//	50,
	//	100,
	//	23,
	//	childWindowHandle,
	//	(HMENU)IDC_LOD_COMBO,
	//	GetModuleHandle(NULL),
	//	NULL);

	//SendMessage(hWndLodDropDown,
	//	WM_SETFONT,
	//	(WPARAM)hfDefault,
	//	MAKELPARAM(FALSE,0));

	//SendMessage(hWndLodDropDown,
	//	CB_ADDSTRING,
	//	NULL,
	//	(LPARAM)"1");

	//SendMessage(hWndLodDropDown,
	//	CB_ADDSTRING,
	//	NULL,
	//	(LPARAM)"2");

	//SendMessage(hWndLodDropDown,
	//	CB_ADDSTRING,
	//	NULL,
	//	(LPARAM)"3");

	//SendMessage(hWndLodDropDown,
	//	CB_SETCURSEL,
	//	(WPARAM)2,
	//	NULL);

	//// Create max LOD distance edit box
	//HWND hWndMaxDistEdit=CreateWindow( 
	//	L"EDIT",
	//	L"1800",
	//	WS_TABSTOP | WS_VISIBLE| WS_CHILD | ES_NUMBER,
	//	50,
	//	100,
	//	75,
	//	23,
	//	childWindowHandle,
	//	(HMENU)IDC_MAX_DIST_EDIT,
	//	GetModuleHandle(NULL),
	//	NULL);

	//SendMessage(hWndMaxDistEdit,
	//	WM_SETFONT,
	//	(WPARAM)hfDefault,
	//	MAKELPARAM(FALSE,0));

	//// Show the window
 //   ShowWindow(childWindowHandle, SW_SHOW);

	// Return the new window handle
	return windowHandle;
}

/// <summary> Performs shutdown and cleanup functions for the application window </summary>
/// <returns> true if it succeeds, false if it fails </returns>
bool ApplicationWindow::Exit()
{
	return true;
}