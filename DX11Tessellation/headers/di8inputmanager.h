#ifndef _DI8INPUTMANAGER_H_
#define _DI8INPUTMANAGER_H_

#define DIRECTINPUT_VERSION 0x0800

#define KEY_DOWN	0x80
#define KEY_PRESSED	0x40
#define KEY_UP		0x20

#include <windows.h>
#include <dinput.h>

class DI8InputManager
{
public:
	~DI8InputManager();

	bool Init(HWND windowHandle, HINSTANCE hInstance);
	bool Update();
	bool Exit();

	bool IsKeyPressed(unsigned char keyIndex);
	bool IsKeyDown(unsigned char keyIndex);
	bool IsKeyUp(unsigned char keyIndex);

	float GetJoystickAxis(unsigned int stick, unsigned int axis);
	bool GetJoystickButton(unsigned int button);

	static DI8InputManager* GetInstance();

	IDirectInput8* GetInputObject() { return inputObject; }
	IDirectInputDevice8* GetJoystickDevice() { return joystickDevice; }
	void SetJoystickDevice(IDirectInputDevice8* joystick) { joystickDevice = joystick; }
private:
	static DI8InputManager* instance;
	DI8InputManager();

	IDirectInput8* inputObject;
	IDirectInputDevice8* keyboardDevice;
	IDirectInputDevice8* joystickDevice;

	DIJOYSTATE2 joystickState;

	unsigned char keyStates[256];
	unsigned char lastKeyStates[256];
};

#endif