#pragma once

//WIN32 API
//#include <windows.h>

#include "Game.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GameTimer.h"

//Essentially, Unicode (http://unicode.org/) uses 16-bit values to represent a character. This allows us to represent a larger character set
//to support international characters, and other symbols.For Unicode in C++, we use the wide - characters m_type wchar_t.In 32 - and 64 -
//bit Windows, a wchar_t is 16 - bits.When using wide characters, we must prefix a string literal with a capital L; for example:
// Configuration Properties -> General -> Project Defaults -> Character Set -> Use Unicode Character Set.

class Window
{
	friend class Game;
	friend class Graphics;
public:
	Window(HINSTANCE instanceHandle, std::wstring WindowName, int show);
	~Window();
	static LRESULT CALLBACK _HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK _HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void CalculateFrameStats();

	bool  Run();

protected:
	//Main window handle
	HWND m_MainWnd = nullptr;
	std::wstring m_MainWndCaption;
	GameTimer m_Timer;

private:
	const wchar_t* m_ClassName = L"FioriFrameWork";
	HINSTANCE m_hInst = nullptr;
public:
	Keyboard kbd;
	Mouse mouse;
public:
};