#pragma once

//WIN32 API
//#include <windows.h>

#include "Graphics.h"

//Essentially, Unicode (http://unicode.org/) uses 16-bit values to represent a character. This allows us to represent a larger character set
//to support international characters, and other symbols.For Unicode in C++, we use the wide - characters type wchar_t.In 32 - and 64 -
//bit Windows, a wchar_t is 16 - bits.When using wide characters, we must prefix a string literal with a capital L; for example:
// Configuration Properties -> General -> Project Defaults -> Character Set -> Use Unicode Character Set.

class Window
{
	friend Graphics::Graphics(Window& window);
	
public:
	Window(HINSTANCE instanceHandle, int show);
	~Window();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int Run();

protected:
	//Main window handle
	HWND m_MainWnd = nullptr;

private:
	const wchar_t* m_ClassName = L"FioriFrameWork";
	HINSTANCE m_hInst = nullptr;




};
