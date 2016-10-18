#include "System.h"

System::System()
{
	// any base module construction happens here.
}

System::System(const System &)
{
}

System::~System()
{
}

bool System::Initialize()
{
	int screenWidth, screenHeight;
	bool result = true;
	
	// initialize screen variables to 0
	screenWidth = screenHeight = 0;

	// initialize the windows API - pass in references to screen variables.
	InitializeWindow(screenWidth, screenHeight);
	
	// we will do further initialization here of other modules

	return result;
}

void System::Shutdown()
{
	ShutdownWindow();
}

void System::Run()
{
	MSG msg;
	bool done, result;

	// wipe out the memory that sits at the location of msg.
	ZeroMemory(&msg, sizeof(MSG));

	// enter an infinite loop until the window or users says to quit.
	done = false;
	while (!done) 
	{
		// handle the window messages
		// PeekMessage is non-blocking, it does not wait for a message to appear in the queue, just peeks and exits
		// if there is a message, it fills the variable msg
		// we want it to check all windows attached to the thread, so we dont specify a windows handle
		// by leaving wMsgFilterMin and wMsgFilterMax at 0 we tell it to return all messages
		// PM_REMOVE specifies that messages should be removed from the queue having been dealt with.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); // convert any virtual-key messages into character messages
			DispatchMessage(&msg); // send the message to our windows procedure 
		}

		// check for the quit message (user hitting alt-f4 or pressing the close button on the window)
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else 
		{
			// otherwise, handle the current frame
			result = Frame();
			if (!result)
				done = true;
		}
	}
}

LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_KEYDOWN:
	{
		return 0;
	}
	case WM_KEYUP:
	{
		return 0;
	}
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

bool System::Frame()
{
	bool result = true;

	// this is where we would update our input system

	// and doing any rendering.

	return result;
}

void System::InitializeWindow(int& screenWidth, int& screenHeight)
{
	// build the window to actually display
	WNDCLASSEX wc; // WNDCLASSEX contains a lot of info to setup the window
	DEVMODE dmScreenSettings; // DEVMODE contains info about the display device
	int posX, posY;

	// get an external pointer to the object
	ApplicationHandle = this;

	// get a handle to the instance of the application
	hInstance = GetModuleHandle(NULL);

	// set a name - the L means we're encoding the string as a 16 bit char (wchar_t)
	applicationName = L"Generic Engine";

	// set the window class values: https://msdn.microsoft.com/library/windows/desktop/ms633577
	// HREDRAW & VREDRAW - redraw the entire window if a movement or size adjustment changes the size of the client area
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc; // function pointer to the windows procedure
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = applicationName;
	wc.lpszMenuName = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);

	// register the window class we just setup
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Window Registration Failed", L"Error", MB_ICONEXCLAMATION | MB_OK);
	}

	// find the desktop resolution
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN) {
		// set the screen to the max size of the user desktop and enforce 32 bit
		// Pel here is short for pixel.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// make those changes actually happen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// set the window position to the top left corner
		posX = posY = 0;
	}
	else 
	{
		// otherwise, we'll default to 800x600
		screenWidth = 800;
		screenHeight = 600;

		// position the window at the middle of the display
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// create the window using those settings
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms632680(v=vs.85).aspx
	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, applicationName, applicationName,
		WS_OVERLAPPEDWINDOW,
		posX, posY, screenWidth, screenHeight, NULL, NULL, hInstance, NULL);


	if (hWnd == NULL)
	{
		int error = GetLastError();
		MessageBox(NULL, L"Error creating window", L"Error", MB_ICONEXCLAMATION | MB_OK);
	}

	// bring the window up and give it focus
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	ShowCursor(false);
}

void System::ShutdownWindow()
{
	// shutting down the window
	ShowCursor(true);

	if (FULL_SCREEN)
	{
		// if we're full screen, clear the current display settings
		ChangeDisplaySettings(NULL, 0);
	}

	// remove the window
	DestroyWindow(hWnd);
	hWnd = NULL;

	// remove the application instance
	UnregisterClass(applicationName, hInstance);
	hInstance = NULL;

	// release the pointer
	ApplicationHandle = NULL;
}

// our windows procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Hello, World!");

	switch (umessage) 
	{
		// we handle destroy and close to trigger the window to close
		case WM_DESTROY: 
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);

			// Here your application is laid out.
			// For this introduction, we just print out "Hello, World!"
			// in the top left corner.
			TextOut(hdc,
				5, 5,
				greeting, _tcslen(greeting));
			// End application-specific layout section.

			EndPaint(hwnd, &ps);
			break;
		}
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
