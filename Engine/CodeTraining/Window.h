#pragma once

#include <Windows.h>

class Window
{
public:
	Window(HINSTANCE hInstance, int show);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int Run();
	~Window();

protected:
	HWND m_hWnd;
private:
	HINSTANCE m_hInstance;
	const wchar_t* m_ClassName = L"FioriFramework";
};
