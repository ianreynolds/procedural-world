#include "di8inputmanager.h"

DI8InputManager* DI8InputManager::instance;

DI8InputManager* DI8InputManager::GetInstance()
{
	if(!instance)
	{
		instance = new DI8InputManager();
	}

	return instance;
}

BOOL CALLBACK EnumCallback(const DIDEVICEINSTANCE* instance, VOID* context)
{
    HRESULT hr;

	IDirectInputDevice8* joystickDevice = DI8InputManager::GetInstance()->GetJoystickDevice();

    // Obtain an interface to the enumerated joystick.
	hr = DI8InputManager::GetInstance()->GetInputObject()->CreateDevice(instance->guidInstance, &joystickDevice, NULL);

    // If it failed, then we can't use this joystick. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if (FAILED(hr)) { 
        return DIENUM_CONTINUE;
    }

	DI8InputManager::GetInstance()->SetJoystickDevice(joystickDevice);

    // Stop enumeration. Note: we're just taking the first joystick we get. You
    // could store all the enumerated joysticks and let the user pick.
    return DIENUM_STOP;
}

BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context)
{
	HRESULT hResult;
    HWND hDlg = (HWND)context;

    DIPROPRANGE propRange; 
    propRange.diph.dwSize       = sizeof(DIPROPRANGE); 
    propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
    propRange.diph.dwHow        = DIPH_BYID; 
    propRange.diph.dwObj        = instance->dwType;
    propRange.lMin              = -1000; 
    propRange.lMax              = +1000; 
    
    // Set the range for the axis
    hResult = DI8InputManager::GetInstance()->GetJoystickDevice()->SetProperty(DIPROP_RANGE, &propRange.diph);
    if(FAILED(hResult))
	{
        return DIENUM_STOP;
    }

    return DIENUM_CONTINUE;
}

DI8InputManager::DI8InputManager()
	: keyboardDevice(NULL), joystickDevice(NULL)
{
	inputObject = nullptr;

	ZeroMemory(keyStates, sizeof(unsigned char) * 256);
	ZeroMemory(lastKeyStates, sizeof(unsigned char) * 256);
}

DI8InputManager::~DI8InputManager()
{

}

bool DI8InputManager::Init(HWND windowHandle, HINSTANCE hInstance)
{
	HRESULT hResult;

	hResult = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&inputObject, nullptr);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to create direct input object", L"Input Error", NULL);
		return false;
	}

	hResult = inputObject->CreateDevice(GUID_SysKeyboard, &keyboardDevice, nullptr);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to create keyboard device", L"Input Error", NULL);
		return false;
	}

	hResult = keyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to set keyboard device data format", L"Input Error", NULL);
		return false;
	}

	hResult = keyboardDevice->SetCooperativeLevel(windowHandle, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to set keyboard device cooperative level", L"Input Error", NULL);
		return false;
	}

	hResult = keyboardDevice->Acquire();
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to acquire keyboard device", L"Input Error", NULL);
		return false;
	}

	hResult = inputObject->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumCallback, NULL, DIEDFL_ATTACHEDONLY);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to enumerate joystick device", L"Input Error", NULL);
		return false;
	}

	if(!joystickDevice)
	{
		MessageBox(NULL, L"No joystick device found", L"Input warning", NULL);
	}
	else
	{
		DIDEVCAPS capabilities;

		hResult = joystickDevice->SetDataFormat(&c_dfDIJoystick2);
		if(FAILED(hResult))
		{
			MessageBox(NULL, L"Failed to set joystick device data format", L"Input Error", NULL);
			return false;
		}

		hResult = joystickDevice->SetCooperativeLevel(NULL, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
		if(FAILED(hResult))
		{
			MessageBox(NULL, L"Failed to set joystick device cooperative level", L"Input Error", NULL);
			return false;
		}

		capabilities.dwSize = sizeof(DIDEVCAPS);

		hResult = joystickDevice->GetCapabilities(&capabilities);
		if(FAILED(hResult))
		{
			MessageBox(NULL, L"Failed to get joystick device capabilities", L"Input Error", NULL);
			return false;
		}

		hResult = joystickDevice->EnumObjects(EnumAxesCallback, NULL, DIDFT_AXIS);
		if(FAILED(hResult))
		{
			MessageBox(NULL, L"Failed to enumerate joystick device axes", L"Input Error", NULL);
			return false;
		}

		hResult = joystickDevice->Acquire();
		if(FAILED(hResult))
		{
			MessageBox(NULL, L"Failed to acquire joystick device", L"Input Error", NULL);
			return false;
		}
	}

	return true;
}

bool DI8InputManager::Update()
{
	HRESULT hResult;

	hResult = keyboardDevice->GetDeviceState(sizeof(keyStates), (void*)keyStates);
	if(FAILED(hResult))
	{
		MessageBox(NULL, L"Failed to get keyboard device state", L"Input Error", NULL);
		return false;
	}

	for(int i = 0; i < 256; i++)
	{
		if(keyStates[i])
		{
			if(!(lastKeyStates[i] & KEY_DOWN))
			{
				keyStates[i] = KEY_DOWN | KEY_PRESSED;
			}
		}
		else
		{
			if(lastKeyStates[i] & KEY_DOWN)
			{
				keyStates[i] = KEY_UP;
			}
		}
	}

	memcpy(lastKeyStates, keyStates, sizeof(unsigned char) * 256);

	if(joystickDevice)
	{
		hResult = joystickDevice->Poll();
		if(FAILED(hResult))
		{
			MessageBox(NULL, L"Failed to poll joystick device", L"Input Error", NULL);
			return false;
		}

		hResult = joystickDevice->GetDeviceState(sizeof(DIJOYSTATE2), &joystickState);
		if(FAILED(hResult))
		{
			MessageBox(NULL, L"Failed to get joystick device state", L"Input Error", NULL);
			return false;
		}
	}

	return true;
}

bool DI8InputManager::Exit()
{
	if(inputObject)
	{
		inputObject->Release();
		inputObject = nullptr;
	}

	return true;
}

bool DI8InputManager::IsKeyPressed(unsigned char keyIndex)
{
	if(keyStates[keyIndex] & KEY_PRESSED)
	{
		return true;
	}

	return false;
}

bool DI8InputManager::IsKeyDown(unsigned char keyIndex)
{
	if(keyStates[keyIndex] & KEY_DOWN)
	{
		return true;
	}

	return false;
}

bool DI8InputManager::IsKeyUp(unsigned char keyIndex)
{
	if(keyStates[keyIndex] & KEY_UP)
	{
		return true;
	}

	return false;
}

float DI8InputManager::GetJoystickAxis(unsigned int stick, unsigned int axis)
{
	if(joystickDevice)
	{
		if(stick == 0)
		{
			switch(axis)
			{
			case 0:
				return static_cast<float>(joystickState.lX) / 1000.0f;
				break;
			case 1:
				return static_cast<float>(joystickState.lY) / 1000.0f;
				break;
			default:
				break;
			}
		}
		else if(stick == 1)
		{
			switch(axis)
			{
			case 0:
				return static_cast<float>(joystickState.lRz) / 1000.0f;
				break;
			case 1:
				return static_cast<float>(joystickState.lZ) / 1000.0f;
				break;
			default:
				break;
			}
		}
	}

	return 0.0f;
}

bool DI8InputManager::GetJoystickButton(unsigned int button)
{
	if(joystickState.rgbButtons[button])
	{
		return true;
	}

	return false;
}