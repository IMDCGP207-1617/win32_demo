#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

const bool FULL_SCREEN = false;

// flag the class as final (C++11 required) to stop anything from deriving from it.
class System final
{
public:
	System(); // constructor
	System(const System&); // copy constructor
	~System(); // destructor

	bool Initialize();
	void Shutdown();
	void Run();

	// windows message handler - windows handle, the message, additional message info (wparam, lparam)
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:

	bool Frame();
	void InitializeWindow(int&, int&);
	void ShutdownWindow();

	LPCWSTR applicationName;
	HINSTANCE hInstance;
	HWND hWnd;

};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static System* ApplicationHandle = NULL;